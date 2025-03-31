//
// Created by YongGyu Lee on 2024. 6. 15..
//

#ifndef PREVIEW_TEST_GTEST_H_
#define PREVIEW_TEST_GTEST_H_

#include <gtest/gtest.h>

#include <type_traits>

#include "preview/__core/cxx_version.h"

#define VERSION_CAT2(x, y) x ## _CXXSTD_ ## y
#define VERSION_CAT(x, y) VERSION_CAT2(x, y)

#define VERSIONED(name) VERSION_CAT(name, PREVIEW_CXX_VERSION)

#define EXPECT_EQ_TYPE(...) \
    EXPECT_TRUE((std::is_same<__VA_ARGS__>::value))

#define EXPECT_STRONG_EQ(x, y) \
    EXPECT_EQ_TYPE(decltype(x), decltype(y)); \
    EXPECT_EQ(x, y)

#define EXPECT_TRUE_TYPE(...) \
    EXPECT_TRUE((__VA_ARGS__::value))

#define EXPECT_FALSE_TYPE(...) \
    EXPECT_FALSE((__VA_ARGS__::value))

#define VERSIONED_TYPE_TEST(CaseName, TestName) \
    VERSIONED_TYPE_TEST_IMPL(CaseName, VERSIONED(TestName))

#define VERSIONED_TYPE_TEST_IMPL(CaseName, TestName) \
    TYPED_TEST(CaseName, TestName)

class CVNameGenerator {
 public:
  template <typename T>
  static std::string GetName(int) {
    std::string cv;
    if (std::is_const<T>::value) cv = "const ";
    if (std::is_volatile<T>::value) cv = "volatile ";
    return cv + ::testing::internal::GetTypeName<T>();
  }
};

#define ASSERT_NOEXCEPT(...) \
    static_assert(noexcept(__VA_ARGS__), "")

#endif //PREVIEW_TEST_GTEST_H_
