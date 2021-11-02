#include <thread>
#include <vector>
#include <unistd.h>
#include <atomic>
#include <condition_variable>

#include "boat.h"

namespace proj2 {

std::atomic<bool> finish;

std::atomic<int> child_on_oahu, adult_on_oahu;
std::atomic<int> child_on_boat;
std::atomic<bool> boat_at_oahu;

std::mutex child_oahu_lock;
std::condition_variable child_oahu_cv;
std::mutex child_molokai_lock;
std::condition_variable child_molokai_cv;
std::mutex adult_oahu_lock;
std::condition_variable adult_oahu_cv;

Boat::Boat() {
    finish = false;
    child_on_oahu = 0;
    adult_on_oahu = 0;
    child_on_boat = 0;
    boat_at_oahu = true;
}

/*
algorithm:
    if at oahu {
        when boat comes, if there is at least 2 chidren, then grab for boat to molokai
        if becomes passenger, then drive back the boat when arrive at molokai
    } else {
        wait for boat comes, then grab for boat to oahu
    }
*/
void child_routine(BoatGrader *bg) {
    child_on_oahu++;
    bool in_oahu = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    while (!finish) {
        if (in_oahu) {
            std::unique_lock<std::mutex> lock(child_oahu_lock);
            child_oahu_cv.wait(lock, []{ return boat_at_oahu && child_on_oahu + child_on_boat >= 2 && child_on_boat < 2; });
            if (child_on_boat == 0) {
                child_on_boat = 1;
                child_on_oahu--;
                in_oahu = false;
                bg->ChildRowToMolokai();
            } else {
                if (child_on_oahu <= 1 && adult_on_oahu == 0) {
                    finish = true;
                }
                bg->ChildRideToMolokai();
                if (!finish) {
                    bg->ChildRowToOahu();
                    child_on_boat = 0;
                    adult_oahu_cv.notify_one();
                } else {
                    child_molokai_cv.notify_one();
                }
            }
            child_oahu_cv.notify_one();
        } else {
            std::unique_lock<std::mutex> lock(child_molokai_lock);
            child_molokai_cv.wait(lock, []{ return finish || !boat_at_oahu; });
            if (!finish) {
                bg->ChildRowToOahu();
                boat_at_oahu = true;
                in_oahu = true;
                child_on_oahu++;
                child_oahu_cv.notify_one();
            } else {
                child_molokai_cv.notify_one();
            }
        }
    }
}

void adult_routine(BoatGrader *bg) {
    adult_on_oahu++;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::unique_lock<std::mutex> lock(adult_oahu_lock);
    adult_oahu_cv.wait(lock, []{ return boat_at_oahu && child_on_oahu + child_on_boat < 2; });
    adult_on_oahu--;
    bg->AdultRowToMolokai();
    boat_at_oahu = false;
    child_molokai_cv.notify_one();
    lock.unlock();
}

void Boat::begin(int a, int b, BoatGrader *bg) {
    std::vector<std::thread> threads;
    for (int i = 0; i < a; i++) {
        threads.emplace_back(adult_routine, bg);
    }
    for (int i = 0; i < b; i++) {
        threads.emplace_back(child_routine, bg);
    }
    while (!threads.empty()) {
        threads.back().join();
        threads.pop_back();
    }
} 

} // namespace proj2
