//
// Created by YongGyu Lee on 2024. 6. 13..
//

#include "preview/__type_traits/conjunction.h"

#include <type_traits>

#include "../gtest.h"

template<typename T>
struct NumberOnly {
  static constexpr int value = T{};
};

TEST(VERSIONED(conjunction), basics) {
  EXPECT_TRUE((preview::conjunction_v<std::is_integral<int>, std::is_floating_point<float>>));
  EXPECT_FALSE((preview::conjunction_v<std::is_integral<int>, std::is_floating_point<int>>));

  EXPECT_TRUE(preview::conjunction_v<>);
  EXPECT_TRUE((preview::conjunction_v<std::true_type, std::true_type, std::true_type>));
  EXPECT_FALSE((preview::conjunction_v<std::false_type, std::false_type, std::false_type>));
  EXPECT_FALSE((preview::conjunction_v<std::false_type, std::false_type, std::false_type, std::true_type>));
  EXPECT_FALSE((preview::conjunction_v<std::false_type, void, void()&&>));
}

TEST(VERSIONED(conjunction), instantiate) {
  EXPECT_FALSE((preview::conjunction_v<std::is_integral<float>, NumberOnly<void>>));
}
