//
// Created by YongGyuLee on 2025-03-02.
//

#include "preview/atomic.h"

#include <memory>

#include "../test_utils.h"

template<typename T>
using using_std_atomic = std::is_base_of<std::atomic<typename T::value_type>, T>;

template<typename T>
class Atomic : public testing::Test {};
using Types = testing::Types<
    bool,
    // volatile bool,
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
      << "preview::atomic<" << ::testing::internal::GetTypeName<TypeParam>() << "> implementation: "
      << (using_std_atomic<preview::atomic<TypeParam>>::value ? "std::atomic" : "Boost.Atomic")
      << std::endl;

  std::cout
      << "preview::atomic<" << ::testing::internal::GetTypeName<TypeParam>() << ">::is_always_lock_free: "
      << std::boolalpha << preview::atomic<TypeParam>::is_always_lock_free
      << std::endl;

  SUCCEED();
}
