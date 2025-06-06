//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// <mdspan>

#include <cassert>
#include <array>

#include "preview/mdspan.h"
#include "preview/span.h"

#include "../ConvertibleToIntegral.h"

// Helper file to implement combinatorial testing of extents constructor
//
// preview::extents can be constructed from just indices, a std::array, or a std::span
// In each of those cases one can either provide all extents, or just the dynamic ones
// If constructed from std::span, the span needs to have a static extent
// Furthermore, the indices/array/span can have integer types other than index_type

template <class E, class AllExtents>
testing::AssertionResult test_runtime_observers(E ext, AllExtents expected) {
  for (typename E::rank_type r = 0; r < ext.rank(); r++) {
    EXPECT_EQ_TYPE(decltype(ext.extent(0)), typename E::index_type);
    ASSERT_NOEXCEPT(ext.extent(0));
    if (ext.extent(r) != static_cast<typename E::index_type>(expected[r])) {
      return testing::AssertionFailure()
           << ext.extent(r) << " != " << static_cast<typename E::index_type>(expected[r])
          << ", with ext = " << testing::PrintToString(ext)
          << ", expected = " << testing::PrintToString(expected)
          << ", r = " << r;
    }
  }
  return testing::AssertionSuccess();
}

template <class E, class AllExtents>
testing::AssertionResult test_implicit_construction_call(E e, AllExtents all_ext) {
  return test_runtime_observers(e, all_ext);
}

template <class E, class Test, class AllExtents>
testing::AssertionResult test_construction(AllExtents all_ext) {
  // test construction from all extents
  auto result = Test::template test_construction<E>(all_ext, all_ext, std::make_index_sequence<E::rank()>());
  if (!result) {
    return result << " while testing construction from all extents";
  }

  // test construction from just dynamic extents
  // create an array of just the extents corresponding to dynamic values
  std::array<typename AllExtents::value_type, E::rank_dynamic()> dyn_ext{};
  size_t dynamic_idx = 0;
  for (size_t r = 0; r < E::rank(); r++) {
    if (E::static_extent(r) == preview::dynamic_extent) {
      dyn_ext[dynamic_idx] = all_ext[r];
      dynamic_idx++;
    }
  }

  result = Test::template test_construction<E>(all_ext, dyn_ext, std::make_index_sequence<E::rank_dynamic()>());;
  if (!result) {
    return result << " while testing construction from just dynamic extents";
  }
  return result;
}

template <class T, class TArg, class Test>
testing::AssertionResult test() {
  testing::AssertionResult result = testing::AssertionSuccess();
  constexpr size_t D = preview::dynamic_extent;

  test_construction<preview::extents<T>, Test>(std::array<TArg, 0>{});

  test_construction<preview::extents<T, 3>, Test>(std::array<TArg, 1>{3});
  test_construction<preview::extents<T, D>, Test>(std::array<TArg, 1>{3});

  test_construction<preview::extents<T, 3, 7>, Test>(std::array<TArg, 2>{3, 7});
  test_construction<preview::extents<T, 3, D>, Test>(std::array<TArg, 2>{3, 7});
  test_construction<preview::extents<T, D, 7>, Test>(std::array<TArg, 2>{3, 7});
  test_construction<preview::extents<T, D, D>, Test>(std::array<TArg, 2>{3, 7});

  test_construction<preview::extents<T, 3, 7, 9>, Test>(std::array<TArg, 3>{3, 7, 9});
  test_construction<preview::extents<T, 3, 7, D>, Test>(std::array<TArg, 3>{3, 7, 9});
  test_construction<preview::extents<T, 3, D, D>, Test>(std::array<TArg, 3>{3, 7, 9});
  test_construction<preview::extents<T, D, 7, D>, Test>(std::array<TArg, 3>{3, 7, 9});
  test_construction<preview::extents<T, D, D, D>, Test>(std::array<TArg, 3>{3, 7, 9});
  test_construction<preview::extents<T, 3, D, 9>, Test>(std::array<TArg, 3>{3, 7, 9});
  test_construction<preview::extents<T, D, D, 9>, Test>(std::array<TArg, 3>{3, 7, 9});
  test_construction<preview::extents<T, D, 7, 9>, Test>(std::array<TArg, 3>{3, 7, 9});

  test_construction<preview::extents<T, 1, 2, 3, 4, 5, 6, 7, 8, 9>, Test>(std::array<TArg, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9});
  test_construction<preview::extents<T, D, 2, 3, D, 5, D, 7, D, 9>, Test>(std::array<TArg, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9});
  test_construction<preview::extents<T, D, D, D, D, D, D, D, D, D>, Test>(std::array<TArg, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9});

  return result;
}

template <class Test>
testing::AssertionResult test_index_type_combo() {
  testing::AssertionResult result = test<int, int, Test>();
  test<int, size_t, Test>();
  test<unsigned, int, Test>();
  test<signed char, size_t, Test>();
  test<long long, unsigned, Test>();
  test<size_t, int, Test>();
  test<size_t, size_t, Test>();
  test<int, IntType, Test>();
  test<signed char, IntType, Test>();
  return result;
}
