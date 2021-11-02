#include "all_instructions.h"

void run_all_instructions(proj1::Instructions instructions, proj1::EmbeddingHolder* users, proj1::EmbeddingHolder* items, bool make_copy) {
    int n_users = users->get_n_embeddings();
    int n_items = items->get_n_embeddings();
    for (proj1::Instruction inst: instructions) {
        if (inst.order == proj1::INIT_EMB) {
            n_users++;
        }
    }
    std::vector<std::unique_ptr<std::shared_mutex>> user_locks;
    std::vector<std::unique_ptr<std::shared_mutex>> item_locks;
    std::vector<std::thread> threads;
    std::mutex output_lock;
    int current_epoch = 0;
    for (int i = 0; i < n_users; ++i) {
        user_locks.emplace_back(new std::shared_mutex());
    }
    for (int i = 0; i < n_items; ++i) {
        item_locks.emplace_back(new std::shared_mutex());
    }
    for (proj1::Instruction inst: instructions) {
        if (inst.order == proj1::INIT_EMB) {
            int length = users->get_emb_length();
            proj1::Embedding* new_user = new proj1::Embedding(length);
            int user_idx = users->append(new_user);
            threads.emplace_back(proj1::init_embedding, inst, users, items, new_user, user_idx);
        }
    }
    while (!threads.empty()) {
        threads.back().join();
        threads.pop_back();
    }
    sort(instructions.begin(), instructions.end(), [&](const proj1::Instruction& lhs, const proj1::Instruction& rhs) {
        int epoch_lhs = -1, epoch_rhs = -1;
        int type_lhs = -1, type_rhs = -1;
        if (lhs.order == proj1::UPDATE_EMB && lhs.payloads.size() > 3) {
            epoch_lhs = lhs.payloads[3];
            type_lhs = 1;
        }
        if (lhs.order == proj1::RECOMMEND) {
            epoch_lhs = lhs.payloads[1] + 1;
            type_lhs = 0;
        }
        if (rhs.order == proj1::UPDATE_EMB && rhs.payloads.size() > 3) {
            epoch_rhs = rhs.payloads[3];
            type_rhs = 1;
        }
        if (rhs.order == proj1::RECOMMEND) {
            epoch_rhs = rhs.payloads[1] + 1;
            type_rhs = 0;
        }
        if (epoch_lhs != epoch_rhs) {
          return epoch_lhs < epoch_rhs;
        } else {
          return type_lhs < type_rhs;
        }
    });
    proj1::EmbeddingHolder* users_copy = new proj1::EmbeddingHolder(users);
    proj1::EmbeddingHolder* items_copy = new proj1::EmbeddingHolder(items);
    for (proj1::Instruction inst: instructions) {
        if (inst.order == proj1::UPDATE_EMB) {
            int user_idx = inst.payloads[0];
            int item_idx = inst.payloads[1];
            int epoch = 0;
            if (inst.payloads.size() > 3) {
                epoch = inst.payloads[3];
            }
            if (current_epoch != epoch) {
                while (!threads.empty()) {
                    threads.back().join();
                    threads.pop_back();
                }
                epoch = current_epoch;
                if (make_copy) {
                    users_copy = new proj1::EmbeddingHolder(users);
                    items_copy = new proj1::EmbeddingHolder(items);
                }
            }
            threads.emplace_back([&user_locks, &item_locks, inst, users, items, user_idx, item_idx] {
                user_locks[user_idx]->lock();
                item_locks[item_idx]->lock();
                proj1::update_embedding(inst, users, items);
                item_locks[item_idx]->unlock();
                user_locks[user_idx]->unlock();
            });
        }
        if (inst.order == proj1::RECOMMEND) {
            int user_idx = inst.payloads[0];
            int epoch = inst.payloads[1] + 1;
            if (current_epoch != epoch) {
                while (!threads.empty()) {
                    threads.back().join();
                    threads.pop_back();
                }
                epoch = current_epoch;
                if (make_copy) {
                    users_copy = new proj1::EmbeddingHolder(users);
                    items_copy = new proj1::EmbeddingHolder(items);
                }
            }
            if (make_copy) {
                threads.emplace_back([&output_lock, inst, users_copy, items_copy] {
                    proj1::Embedding* recommendation = proj1::recommend(inst, users_copy, items_copy);
                    output_lock.lock();
                    recommendation->write_to_stdout();
                    output_lock.unlock();
                });
            } else {
                threads.emplace_back([&user_locks, &item_locks, &output_lock, inst, users, items, user_idx] {
                    user_locks[user_idx]->lock_shared();
                    std::vector<int> payloads(inst.payloads.begin() + 2, inst.payloads.end());
                    std::sort(payloads.begin(), payloads.end());
                    for (int item_idx: payloads) {
                        item_locks[item_idx]->lock_shared();
                    }
                    proj1::Embedding* recommendation = proj1::recommend(inst, users, items);
                    output_lock.lock();
                    recommendation->write_to_stdout();
                    output_lock.unlock();
                    std::reverse(payloads.begin(), payloads.end());
                    for (int item_idx: payloads) {
                        item_locks[item_idx]->unlock_shared();
                    }
                    user_locks[user_idx]->unlock_shared();
                });
            }
        }
    }
    while (!threads.empty()) {
        threads.back().join();
        threads.pop_back();
    }
}
