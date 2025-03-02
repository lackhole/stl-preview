//
// Created by YongGyuLee on 2025-03-02.
//

#include "preview/atomic.h"

#include <memory>

#include "../test_utils.h"

template<typename T>
class Atomic : public testing::Test {};
using Types = testing::Types<
    bool,
    volatile bool,
    int,
    unsigned,
    long,
    unsigned long,
    long long,
    unsigned long long,
    float,
    double,
    std::intptr_t*,
    std::shared_ptr<int>
>;
TYPED_TEST_SUITE(Atomic, Types, CVQualifiedNameGenerator);

VERSIONED_TYPED_TEST(Atomic, Parameters) {
  std::cout
      << "Using boost.atomic<" << ::testing::internal::GetTypeName<TypeParam>() << ">: "
      << std::boolalpha << !std::is_base_of<std::atomic<TypeParam>, preview::atomic<TypeParam>>::value
      << std::endl;

    std::cout
        << "Using boost.atomic<" << ::testing::internal::GetTypeName<TypeParam>() << ">::is_always_lock_free: "
      << std::boolalpha << preview::atomic<TypeParam>::is_always_lock_free
      << std::endl;

  SUCCEED();
}
