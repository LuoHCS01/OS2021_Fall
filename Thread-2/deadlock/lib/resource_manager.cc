#include <set>
#include <mutex>
#include <thread>
#include <chrono>
#include <iostream>
#include <condition_variable>
#include "resource_manager.h"

namespace proj2 {

int ResourceManager::request(RESOURCE r, int amount) {
    if (amount <= 0)  return 1;

    std::unique_lock<std::mutex> lk(this->mux);
    this->cv.wait(lk, [this, r, amount] {
        if (this->resource_amount[r] < amount) {
            return false;
        }
        // grant the request
        this->resource_amount[r] -= amount;
        this->tmgr->alloc[std::this_thread::get_id()][r] += amount;
        std::set<std::thread::id> unfinished;
        for (auto p: this->tmgr->running_status) {
            if (p.second) {
                unfinished.insert(p.first);
            } else {
                // free the unrunning resources
                for (auto q: this->tmgr->alloc[p.first]) {
                    this->resource_amount[q.first] += q.second;
                }
            }
        }
        while (true) {
             bool done = true;
             for (auto tid: unfinished) {
                  bool valid = true;
                  for (auto p: this->tmgr->budget[tid]) {
                      if (p.second - this->tmgr->alloc[tid][p.first] > this->resource_amount[p.first]) {
                          valid = false;
                          break;
                      }
                  }
                  if (valid) {
                      for (auto p: this->tmgr->alloc[tid]) {
                          this->resource_amount[p.first] += p.second;
                      }
                      unfinished.erase(tid);
                      done = false;
                      break;
                  }
             }
             if (done) {
                 break;
             }
        }
        // free all resources
        for (auto tid: unfinished) {
            for (auto p: this->tmgr->alloc[tid]) {
                this->resource_amount[p.first] += p.second;
            }
        }
        // alloc resources
        for (auto p: this->tmgr->alloc) {
            for (auto q: p.second) {
                this->resource_amount[q.first] -= q.second;
            }
        }
        this->resource_amount[r] += amount;
        this->tmgr->alloc[std::this_thread::get_id()][r] -= amount;
        return unfinished.empty();
    });
    this->resource_amount[r] -= amount;
    this->tmgr->alloc[std::this_thread::get_id()][r] += amount;
    return 0;
}

void ResourceManager::release(RESOURCE r, int amount) {
    if (amount <= 0)  return;
    std::unique_lock<std::mutex> lk(this->mux);
    this->resource_amount[r] += amount;
    this->tmgr->alloc[std::this_thread::get_id()][r] -= amount;
    this->tmgr->running_status[std::this_thread::get_id()] = false;
    this->cv.notify_all();
}

void ResourceManager::budget_claim(std::map<RESOURCE, int> budget) {
    // This function is called when some workload starts.
    // The workload will eventually consume all resources it claims
    this->tmgr->running_status[std::this_thread::get_id()] = true;
    this->tmgr->budget[std::this_thread::get_id()] = budget;
}

} // namespace: proj2
