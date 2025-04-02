//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <mdspan>

// constexpr index_type stride(rank_type i) const noexcept;
//
//   Constraints: extents_type::rank() > 0 is true.
//
//   Preconditions: i < extents_type::rank() is true.
//
//   Returns: extents().rev-prod-of-extents(i).

#include <array>
#include <cstdint>
#include <cstdio>

#include "preview/mdspan.h"
#include "preview/span.h" // dynamic_extent

#include "gtest.h"
#include "../print_to.h"

template <class E, class... Args>
constexpr void test_stride(std::array<typename E::index_type, E::rank()> strides, Args... args) {
  using M = preview::layout_right::mapping<E>;
  M m(E(args...));

  ASSERT_NOEXCEPT(m.stride(0));
  for (size_t r = 0; r < E::rank(); r++)
    ASSERT_EQ(strides[r], m.stride(r));
}

TEST(MdspanLayoutRight, VERSIONED(stride)) {
  constexpr size_t D = preview::dynamic_extent;
  test_stride<preview::extents<unsigned, D>>(std::array<unsigned, 1>{1}, 7);
  test_stride<preview::extents<unsigned, 7>>(std::array<unsigned, 1>{1});
  test_stride<preview::extents<unsigned, 7, 8>>(std::array<unsigned, 2>{8, 1});
  test_stride<preview::extents<int64_t, D, 8, D, D>>(std::array<int64_t, 4>{720, 90, 10, 1}, 7, 9, 10);
}
