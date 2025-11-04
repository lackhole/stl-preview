//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <mdspan>

// namespace std {
//   template<class Extents>
//   class layout_right::mapping {
//
//     ...
//     static constexpr bool is_always_unique() noexcept { return true; }
//     static constexpr bool is_always_exhaustive() noexcept { return true; }
//     static constexpr bool is_always_strided() noexcept { return true; }
//
//     static constexpr bool is_unique() noexcept { return true; }
//     static constexpr bool is_exhaustive() noexcept { return true; }
//     static constexpr bool is_strided() noexcept { return true; }
//     ...
//   };
// }

#include <cstddef>
#include <utility>

#include "preview/mdspan.h"
#include "preview/span.h" // dynamic_extent

#include "../../test_utils.h"
#include "../print_to.h"

template <class E>
constexpr void test_layout_mapping_right() {
  using M = preview::layout_right::mapping<E>;
  ASSERT_TRUE(M::is_unique());
  ASSERT_TRUE(M::is_exhaustive());
  ASSERT_TRUE(M::is_strided());
  ASSERT_TRUE(M::is_always_unique());
  ASSERT_TRUE(M::is_always_exhaustive());
  ASSERT_TRUE(M::is_always_strided());
  ASSERT_NOEXCEPT(std::declval<M>().is_unique());
  ASSERT_NOEXCEPT(std::declval<M>().is_exhaustive());
  ASSERT_NOEXCEPT(std::declval<M>().is_strided());
  ASSERT_NOEXCEPT(M::is_always_unique());
  ASSERT_NOEXCEPT(M::is_always_exhaustive());
  ASSERT_NOEXCEPT(M::is_always_strided());
}

TEST(MdspanLayoutRight, VERSIONED(properties)) {
  constexpr size_t D = preview::dynamic_extent;
  test_layout_mapping_right<preview::extents<int>>();
  test_layout_mapping_right<preview::extents<signed char, 4, 5>>();
  test_layout_mapping_right<preview::extents<unsigned, D, 4>>();
  test_layout_mapping_right<preview::extents<size_t, D, D, D, D>>();
}
