//
// Created by YongGyu Lee on 2024. 6. 13..
//

#include "preview/__type_traits/conjunction.h"

#include <type_traits>

#include <gtest/gtest.h>


TEST(HelloTest, BasicAssertions) {
  EXPECT_TRUE((preview::conjunction_v<std::is_integral<int>, std::is_floating_point<float>>));
  EXPECT_FALSE((preview::conjunction_v<std::is_integral<int>, std::is_floating_point<int>>));
}
