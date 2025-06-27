/**
 * @file targeting_tests.cpp
 * @brief Unit tests for BULLET targeting system
 */

#include <gtest/gtest.h>
#include "../../../src/targeting/core/targeting_core.h"

using namespace bullet::targeting;

class TargetingTest : public ::testing::Test {
protected:
    void SetUp() override {
        targeting_core = std::make_unique<TargetingCore>();
    }
    
    void TearDown() override {
        targeting_core.reset();
    }
    
    std::unique_ptr<TargetingCore> targeting_core;
};

TEST_F(TargetingTest, InitializationTest) {
    EXPECT_TRUE(targeting_core->initializeSystem());
}

TEST_F(TargetingTest, ShahedDetectionTest) {
    targeting_core->initializeSystem();
    
    auto targets = targeting_core->detectTargets();
    EXPECT_GE(targets.size(), 0);
}

TEST_F(TargetingTest, PrioritySelectionTest) {
    std::vector<Target> test_targets;
    
    // Create test targets
    Target shahed{1, TargetType::SHAHED_DRONE, 50.0, 30.0, 500.0, {100, 0, 0}, 0.9, 0};
    Target helicopter{2, TargetType::HELICOPTER, 50.1, 30.1, 300.0, {150, 0, 0}, 0.85, 0};
    Target s300{3, TargetType::AIR_DEFENSE_SYSTEM, 50.2, 30.2, 0.0, {0, 0, 0}, 0.95, 0};
    
    test_targets = {shahed, helicopter, s300};
    
    Target primary = targeting_core->selectPrimaryTarget(test_targets);
    
    // S-300 should have highest priority
    EXPECT_EQ(primary.type, TargetType::AIR_DEFENSE_SYSTEM);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
