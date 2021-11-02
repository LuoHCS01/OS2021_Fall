#include <gtest/gtest.h>
#include <bits/stdc++.h>
#include "lib/utils.h"
#include "lib/workload.h"
#include "lib/thread_manager.h"
#include "lib/resource_manager.h"

namespace proj2 {

namespace testing{

void run_thread(ResourceManager* rmgr, std::map<RESOURCE, int> budget,
                std::vector<RESOURCE> order, std::map<RESOURCE, int>* block_time) {
    rmgr->budget_claim(budget);
    for (RESOURCE r: order) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        auto start = std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch()).count();
        rmgr->request(r, budget[r]);
        auto end = std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch()).count();
        block_time->insert(std::pair<RESOURCE, int>(r, end - start));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    for (RESOURCE r: order) {
        rmgr->release(r, budget[r]);
    }
}

class CommonTest : public ::testing::Test {
  protected:
    void SetUp() override {
        tmgr = new ThreadManager();
        srand(time(0));
    }
    ThreadManager *tmgr;
    ResourceManager *rmgr;
    std::map<RESOURCE, int> init_resource;
    std::map<RESOURCE, int> budget;
};

TEST_F(CommonTest, test_block) {
    std::cout << "--------------- Test Block ---------------" << std::endl;
    for (RESOURCE r = GPU; r <= NETWORK; r = RESOURCE(r + 1)) {
        init_resource[r] = 8;
    }
    rmgr = new ResourceManager(tmgr, init_resource);
    for (RESOURCE r = GPU; r <= NETWORK; r = RESOURCE(r + 1)) {
        std::map<RESOURCE, int> budget;
        std::vector<RESOURCE> order;
        budget[r] = 3;
        order.emplace_back(r);
        std::vector<std::thread> threads;
        std::map<RESOURCE, int> block_time[3];
        int sleep_time[3];
        for (int i = 0; i < 3; i++) {
            sleep_time[i] = 10 * (rand() % 5 + 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time[i]));
            threads.emplace_back(run_thread, rmgr, budget, order, &block_time[i]);
        }
        while (!threads.empty()) {
            threads.back().join();
            threads.pop_back();
        }
        EXPECT_LT(150 - sleep_time[0] - sleep_time[1], block_time[2][r]);
        EXPECT_LT(block_time[2][r], 250 - sleep_time[0] - sleep_time[1]);
    }
}

TEST_F(CommonTest, test_no_deadlock) {
    std::cout << "------------ Test No Deadlock ------------" << std::endl;
    for (RESOURCE r = GPU; r <= NETWORK; r = RESOURCE(r + 1)) {
        init_resource[r] = 3;
    }
    rmgr = new ResourceManager(tmgr, init_resource);
    std::vector<std::thread> threads;
    std::map<RESOURCE, int> budget[4];
    std::vector<RESOURCE> order[4];
    std::map<RESOURCE, int> block_time[4];
    int i = 0;
    for (RESOURCE r = GPU; r <= NETWORK; r = RESOURCE(r + 1)) {
        RESOURCE r_next;
        if (r == NETWORK) {
            r_next = GPU;
        } else {
            r_next = RESOURCE(r + 1);
        }
        budget[i][r] = 2;
        budget[i][r_next] = 2;
        order[i].emplace_back(r);
        order[i].emplace_back(r_next);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        threads.emplace_back(run_thread, rmgr, budget[i], order[i], &block_time[i]);
        i++;
    }
    while (!threads.empty()) {
        threads.back().join();
        threads.pop_back();
    }
}

TEST_F(CommonTest, test_large_workload) {
    std::cout << "---------- Test Large Workload ----------" << std::endl;
    for (RESOURCE r = GPU; r <= NETWORK; r = RESOURCE(r + 1)) {
        init_resource[r] = 30;
    }
    rmgr = new ResourceManager(tmgr, init_resource);
    int NumThreads = 100;
    std::vector<std::thread> threads;
    std::map<RESOURCE, int> budget[NumThreads];
    std::vector<RESOURCE> order[NumThreads];
    std::map<RESOURCE, int> block_time[NumThreads];
    for (int i = 0; i < NumThreads; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        for (RESOURCE r = GPU; r <= NETWORK; r = RESOURCE(r + 1)) {
            budget[i][r] = rand() % 5 + 5;
            order[i].emplace_back(r);
        }
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(order[i].begin(), order[i].end(), std::default_random_engine(seed));
        threads.emplace_back(run_thread, rmgr, budget[i], order[i], &block_time[i]);
    }
    while (!threads.empty()) {
        threads.back().join();
        threads.pop_back();
    }
}
} // namespace testing
} // namespace proj2

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
