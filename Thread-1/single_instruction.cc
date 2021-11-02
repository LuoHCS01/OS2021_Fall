#include "single_instruction.h"

namespace proj1 {
void init_embedding(Instruction inst, EmbeddingHolder* users, EmbeddingHolder* items, Embedding* new_user, int user_idx) {
    std::vector<std::thread> threads;
    EmbeddingGradient* sum_gradient = nullptr;
    std::mutex* mux = new std::mutex();
    for (int item_index: inst.payloads) {
        threads.emplace_back([&sum_gradient, &mux, items, new_user, item_index] {
            Embedding* item_emb = items->get_embedding(item_index);
            EmbeddingGradient* gradient = cold_start(new_user, item_emb);
            mux->lock();
            if (sum_gradient == nullptr) {
                sum_gradient = gradient;
            } else {
                *sum_gradient = *sum_gradient + *gradient;
            }
            mux->unlock();
        });
    }
    while (!threads.empty()) {
        threads.back().join();
        threads.pop_back();
    }
    users->update_embedding(user_idx, sum_gradient, 0.01);
}

void update_embedding(Instruction inst, EmbeddingHolder* users, EmbeddingHolder* items) {
    int user_idx = inst.payloads[0];
    int item_idx = inst.payloads[1];
    int label = inst.payloads[2];
    Embedding* user = users->get_embedding(user_idx);
    Embedding* item = items->get_embedding(item_idx);
    EmbeddingGradient* gradient = calc_gradient(user, item, label);
    users->update_embedding(user_idx, gradient, 0.01);
    gradient = calc_gradient(item, user, label);
    items->update_embedding(item_idx, gradient, 0.001);
}

Embedding* recommend(Instruction inst, EmbeddingHolder* users, EmbeddingHolder* items) {
    int user_idx = inst.payloads[0];
    Embedding* user = users->get_embedding(user_idx);
    std::vector<Embedding*> item_pool;
    for (unsigned int i = 2; i < inst.payloads.size(); ++i) {
        int item_idx = inst.payloads[i];
        item_pool.push_back(items->get_embedding(item_idx));
    }
    Embedding* recommendation = recommend(user, item_pool);
    return recommendation;
}
} // namespace proj1
