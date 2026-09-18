#include "gtest/gtest.h"

// Simple example test
TEST(SampleTest, BasicAssertions) {
  EXPECT_STRNE("hello", "world");
  EXPECT_EQ(7 * 6, 42);
}
