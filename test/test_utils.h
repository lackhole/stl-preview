//
// Created by YongGyu Lee on 2024. 6. 15..
//

#ifndef PREVIEW_TEST_GTEST_H_
#define PREVIEW_TEST_GTEST_H_

#include <gtest/gtest.h>

#include <type_traits>

#include "preview/__core/cxx_version.h"

class CVQualifiedNameGenerator {
 public:
  template <typename T>
  static std::string GetName(int) {
    if (std::is_const<T>::value && std::is_volatile<T>::value) {
      return "const volatile " + ::testing::internal::GetTypeName<T>();
    } else if (std::is_const<T>::value) {
      return "const " + ::testing::internal::GetTypeName<T>();
    } else if (std::is_volatile<T>::value) {
      return "volatile " + ::testing::internal::GetTypeName<T>();
    } else {
      return ::testing::internal::GetTypeName<T>();
    }
  }
};

#define VERSION_CAT2(x, y) x ## _CXXSTD_ ## y
#define VERSION_CAT(x, y) VERSION_CAT2(x, y)

#define VERSIONED(name) VERSION_CAT(name, PREVIEW_CXX_VERSION)

#define VERSIONED_TYPED_TEST(CaseName, TestName) VERSIONED_TYPED_TEST_IMPL(CaseName, VERSIONED(TestName))
#define VERSIONED_TYPED_TEST_IMPL(CaseName, TestName) TYPED_TEST(CaseName, TestName)

#define VERSIONED_TYPED_TEST_P(CaseName, TestName) VERSIONED_TYPED_TEST_P_IMPL(CaseName, VERSIONED(TestName))
#define VERSIONED_TYPED_TEST_P_IMPL(CaseName, TestName) TYPED_TEST_P(CaseName, TestName)

#define EXPECT_EQ_TYPE(type1, type2) \
    EXPECT_TRUE((std::is_same<type1, type2>::value))

#define EXPECT_TRUE_TYPE(...) \
    EXPECT_TRUE((__VA_ARGS__::value))

#define EXPECT_FALSE_TYPE(...) \
    EXPECT_FALSE((__VA_ARGS__::value))

#endif //PREVIEW_TEST_GTEST_H_
