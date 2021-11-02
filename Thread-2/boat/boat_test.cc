#include <gtest/gtest.h>
#include <bits/stdc++.h>
#include "boat.h"
#include "boatGrader.h"

namespace proj2 {

namespace testing{

class CommonTest : public ::testing::Test {
  protected:
    void SetUp() override {
    }
};

TEST_F(CommonTest, test_1) {
    std::cout << "------------- Test 1, 2 children, 1 adult -------------" << std::endl;
    int adults = 1;
    int children = 2;
    proj2::BoatGrader* bg = new proj2::BoatGrader(adults, children);
    proj2::Boat* boat = new proj2::Boat();
    boat->begin(adults, children, bg);
    int k = bg->childrenLeft();
    EXPECT_EQ(k, 0);
    int l = bg->adultsLeft();
    EXPECT_EQ(l, 0);
    delete bg;
    delete boat;
}

TEST_F(CommonTest, test_2) {
    std::cout << "------------ Test 2, 2 children, 100 adults -----------" << std::endl;
    int adults = 100;
    int children = 2;
    proj2::BoatGrader* bg = new proj2::BoatGrader(adults, children);
    proj2::Boat* boat = new proj2::Boat();
    boat->begin(adults, children, bg);
    int k = bg->childrenLeft();
    EXPECT_EQ(k, 0);
    int l = bg->adultsLeft();
    EXPECT_EQ(l, 0);
    delete bg;
    delete boat;
}

TEST_F(CommonTest, test_3) {
    std::cout << "------------ Test 3, 100 children, 0 adult ------------" << std::endl;
    int adults = 0;
    int children = 100;
    proj2::BoatGrader* bg = new proj2::BoatGrader(adults, children);
    proj2::Boat* boat = new proj2::Boat();
    boat->begin(adults, children, bg);
    int k = bg->childrenLeft();
    EXPECT_EQ(k, 0);
    int l = bg->adultsLeft();
    EXPECT_EQ(l, 0);
    delete bg;
    delete boat;
}

TEST_F(CommonTest, test_4) {
    std::cout << "------------ Test 4, 100 children, 1 adult ------------" << std::endl;
    int adults = 1;
    int children = 100;
    proj2::BoatGrader* bg = new proj2::BoatGrader(adults, children);
    proj2::Boat* boat = new proj2::Boat();
    boat->begin(adults, children, bg);
    int k = bg->childrenLeft();
    EXPECT_EQ(k, 0);
    int l = bg->adultsLeft();
    EXPECT_EQ(l, 0);
    delete bg;
    delete boat;
}

TEST_F(CommonTest, test_5) {
    std::cout << "------------ Test 5, 100 children, 100 adults -----------" << std::endl;
    int adults = 100;
    int children = 100;
    proj2::BoatGrader* bg = new proj2::BoatGrader(adults, children);
    proj2::Boat* boat = new proj2::Boat();
    boat->begin(adults, children, bg);
    int k = bg->childrenLeft();
    EXPECT_EQ(k, 0);
    int l = bg->adultsLeft();
    EXPECT_EQ(l, 0);
    delete bg;
    delete boat;
}

} // namespace testing
} // namespace proj2

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
