#include <gtest/gtest.h>
#include <bits/stdc++.h>
#include <chrono>
#include "single_instruction.h"
#include "all_instructions.h"

namespace proj1 {

using EmbeddingVectors = std::vector<std::vector<double>>;

EmbeddingVectors get_output(std::string filename) {
    std::string line;
    std::ifstream ifs(filename);
    EmbeddingVectors embeddings;
    if (ifs.is_open()) {
        while (std::getline(ifs, line)) {
            std::string parse(line.begin() + 8, line.end());
            for (char& c: parse) {
                if (c == ',') {
                    c = ' ';
                }
            }
            std::vector<double> embedding;
            std::stringstream ss(parse);
            double input;
            while (ss >> input) {
                embedding.push_back(input);
            }
            if (!embedding.empty()) {
                embeddings.push_back(embedding);
            }
        }
        ifs.close();
    } else {
        throw std::runtime_error("Error opening file " + filename + "!");
    }
    return embeddings;
}

bool compare(const EmbeddingVectors &vec1, const EmbeddingVectors &vec2) {
    int n = vec1.size();
    std::vector<int> p(n);
    for (int i = 0; i < n; ++i) {
        p[i] = i;  
    }
    bool equal = false;
    do {
        bool valid = true;
        for (int i = 0; i < n; ++i) {
            if (vec1[i] != vec2[p[i]]) {
                valid = false;
            }
        }
        if (valid) {
            equal = true;
        }
    } while (next_permutation(p.begin(), p.end()));
    return equal;
}

namespace testing{

class CommonTest : public ::testing::Test {
  protected:
    void SetUp() override {
        users_static = new EmbeddingHolder("test_data/users.in");
        items_static = new EmbeddingHolder("test_data/items.in");
    }
    EmbeddingHolder* users_static;
    EmbeddingHolder* items_static;
};

// Test get_output function, omitted in final test
/*
TEST_F(CommonTest, test_get_output){
    EmbeddingVectors embeddings = get_output("format.out");
    for (std::vector<double> embedding: embeddings) {
        for (double value: embedding) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
}
*/
TEST_F (CommonTest, test_single_instruction){
    // 1 init, 2 updates, 2 recommends
    std::cout << "---------- Test Single Instruction, 3 instrs --------" << std::endl;
    EmbeddingHolder* users = users_static;
    EmbeddingHolder* items = items_static;
    Instructions instructions = read_instructrions("test_data/instruction_1.tsv");
    freopen("test_data/output_1.out", "w", stdout);
    for (Instruction instr: instructions) {
        run_all_instructions({instr}, users, items, false);
    }
    users->write_to_stdout();
    items->write_to_stdout();
    freopen("/dev/tty", "w", stdout);
    EXPECT_EQ(system("diff test_data/output_1.out test_data/expect_1.out"), 0);
}

TEST_F (CommonTest, test_concurrency_1){
    std::cout << "------- Test Concurrency 1, 1 init, 3 updates -------" << std::endl;
    EmbeddingHolder* users = users_static;
    EmbeddingHolder* items = items_static;
    Instructions instructions = read_instructrions("test_data/instruction_2.tsv");
    freopen("test_data/output_2.out", "w", stdout);
    run_all_instructions(instructions, users, items, false);
    users->write_to_stdout();
    items->write_to_stdout();
    freopen("/dev/tty", "w", stdout);
    int x[6];
    x[0] = system("diff test_data/output_2.out test_data/expect_2_1.out");
    x[1] = system("diff test_data/output_2.out test_data/expect_2_2.out");
    x[2] = system("diff test_data/output_2.out test_data/expect_2_3.out");
    x[3] = system("diff test_data/output_2.out test_data/expect_2_4.out");
    x[4] = system("diff test_data/output_2.out test_data/expect_2_5.out");
    x[5] = system("diff test_data/output_2.out test_data/expect_2_6.out");
    int ifequal = !x[0] || !x[1] || !x[2] || !x[3] || !x[4] || !x[5];
    EXPECT_EQ(ifequal, 1);
}

TEST_F (CommonTest, test_concurrency_2){
    std::cout << "------- Test Concurrency 2, 1 update, 10 inits ------" << std::endl;
    EmbeddingHolder* users = users_static;
    EmbeddingHolder* items = items_static;
    Instructions instructions = read_instructrions("test_data/instruction_3.tsv");
    freopen("test_data/output_3.out", "w", stdout);
    run_all_instructions(instructions, users, items, false);
    users->write_to_stdout();
    freopen("/dev/tty", "w", stdout);
    EmbeddingVectors real = get_output("test_data/output_3.out");
    EmbeddingVectors expected = get_output("test_data/expect_3.out");
    int ifequal = 1;
    for (int i = 0; i < 10; i++) {
        ifequal = ifequal && (real[6 + i] == expected[i] || real[6 + i] == expected[10 + i]);
    }
    EXPECT_EQ(ifequal, 1);
}

TEST_F (CommonTest, test_epoch_1){
    // Every epoch contains 1 update
    std::cout << "-------------- Test Epoch 1, 3 epoches --------------" << std::endl;
    EmbeddingHolder* users = users_static;
    EmbeddingHolder* items = items_static;
    Instructions instructions = read_instructrions("test_data/instruction_4.tsv");
    freopen("test_data/output_4.out", "w", stdout);
    run_all_instructions(instructions, users, items, false);
    users->write_to_stdout();
    items->write_to_stdout();
    freopen("/dev/tty", "w", stdout);
    EXPECT_EQ(system("diff test_data/output_4.out test_data/expect_4.out"), 0);
}

TEST_F (CommonTest, test_epoch_2){
    // Contain 2, 1, 2, 1, 1 updates
    std::cout << "-------------- Test Epoch 2, 5 epoches --------------" << std::endl;
    EmbeddingHolder* users = users_static;
    EmbeddingHolder* items = items_static;
    Instructions instructions = read_instructrions("test_data/instruction_5.tsv");
    freopen("test_data/output_5.out", "w", stdout);
    run_all_instructions(instructions, users, items, false);
    users->write_to_stdout();
    items->write_to_stdout();
    freopen("/dev/tty", "w", stdout);
    int x[4];
    x[0] = system("diff test_data/output_5.out test_data/expect_5_1.out");
    x[1] = system("diff test_data/output_5.out test_data/expect_5_2.out");
    x[2] = system("diff test_data/output_5.out test_data/expect_5_3.out");
    x[3] = system("diff test_data/output_5.out test_data/expect_5_4.out");
    int ifequal = !x[0] || !x[1] || !x[2] || !x[3];
    EXPECT_EQ(ifequal, 1);
}

TEST_F (CommonTest, test_recommend_copy){
    // 8 epoches of 8 update, 10 recommends
    std::cout << "------------ Test Recommend, 10 recommends -----------" << std::endl;
    EmbeddingHolder* users = users_static;
    EmbeddingHolder* items = items_static;
    Instructions instructions = read_instructrions("test_data/instruction_6.tsv");
    freopen("test_data/output_6.out", "w", stdout);
    run_all_instructions(instructions, users, items, true);
    users->write_to_stdout();
    items->write_to_stdout();
    freopen("/dev/tty", "w", stdout);
    EmbeddingVectors real = get_output("test_data/output_6.out");
    EmbeddingVectors expect = get_output("test_data/expect_6.out");
    EmbeddingVectors real_recommend{&real[0], &real[10]};
    EmbeddingVectors expect_recommend{&expect[0], &expect[10]};
    EXPECT_EQ(compare(real_recommend, expect_recommend), 1);
    EmbeddingVectors real_useritem{&real[10], &real[22]};
    EmbeddingVectors expect_useritem{&expect[10], &expect[22]};
    int ifequal = 1;
    for (unsigned int i = 0; i < real_useritem.size(); i++) {
        ifequal = ifequal && (real_useritem[i] == expect_useritem[i]);
    }
    EXPECT_EQ(ifequal, 1);
}

TEST_F (CommonTest, test_recommend_inplace){
    // 8 epoches of 8 update, 10 recommends
    std::cout << "------------ Test Recommend, 10 recommends -----------" << std::endl;
    EmbeddingHolder* users = users_static;
    EmbeddingHolder* items = items_static;
    Instructions instructions = read_instructrions("test_data/instruction_6.tsv");
    freopen("test_data/output_6.out", "w", stdout);
    run_all_instructions(instructions, users, items, false);
    users->write_to_stdout();
    items->write_to_stdout();
    freopen("/dev/tty", "w", stdout);
    EmbeddingVectors real = get_output("test_data/output_6.out");
    EmbeddingVectors expect = get_output("test_data/expect_6.out");
    EmbeddingVectors real_recommend{&real[0], &real[10]};
    EmbeddingVectors expect_recommend{&expect[0], &expect[10]};
    EXPECT_EQ(compare(real_recommend, expect_recommend), 1);
    EmbeddingVectors real_useritem{&real[10], &real[22]};
    EmbeddingVectors expect_useritem{&expect[10], &expect[22]};
    int ifequal = 1;
    for (unsigned int i = 0; i < real_useritem.size(); i++) {
        ifequal = ifequal && (real_useritem[i] == expect_useritem[i]);
    }
    EXPECT_EQ(ifequal, 1);
}

TEST_F (CommonTest, test_efficiency_copy) {
    // large data, measure the running time
    std::cout << "------------- Test Efficiency, large data ------------" << std::endl;
    EmbeddingHolder* users = new EmbeddingHolder("test_data/large_users.in");
    EmbeddingHolder* items = new EmbeddingHolder("test_data/large_items.in");
    Instructions instructions = read_instructrions("test_data/large_instruction.tsv");
    auto start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    run_all_instructions(instructions, users, items, true);
    auto end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout << "************************************" << std::endl;
    std::cout << "     Running Time: " << end - start << " ms" << std::endl;
    std::cout << "************************************" << std::endl;
}

TEST_F (CommonTest, test_efficiency_inplace) {
    // large data, measure the running time
    std::cout << "------------- Test Efficiency, large data ------------" << std::endl;
    EmbeddingHolder* users = new EmbeddingHolder("test_data/large_users.in");
    EmbeddingHolder* items = new EmbeddingHolder("test_data/large_items.in");
    Instructions instructions = read_instructrions("test_data/large_instruction.tsv");
    auto start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    run_all_instructions(instructions, users, items, false);
    auto end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout << "************************************" << std::endl;
    std::cout << "     Running Time: " << end - start << " ms" << std::endl;
    std::cout << "************************************" << std::endl;
}
} // namespace testing
} // namespace proj1

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
