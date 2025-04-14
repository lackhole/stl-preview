//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <mdspan>

// Test default construction:
//
// constexpr mapping() noexcept = default;

#include <cstdint>

#include "preview/mdspan.h"
#include "preview/span.h" // dynamic_extent

#include "../../test_utils.h"
#include "../print_to.h"

template <class E>
constexpr void test_construction() {
  using M = preview::layout_right::mapping<E>;
  ASSERT_NOEXCEPT(M{});
  M m;
  E e;

  // check correct extents are returned
  ASSERT_NOEXCEPT(m.extents());
  EXPECT_EQ(m.extents(), e);
  EXPECT_EQ(e, m.extents());

  // check required_span_size()
  typename E::index_type expected_size = 1;
  for (typename E::rank_type r = 0; r < E::rank(); r++)
    expected_size *= e.extent(r);
  EXPECT_EQ(m.required_span_size(), expected_size);
}

TEST(MdSpanLayoutRight, VERSIONED(ctor_default)) {
  constexpr size_t D = preview::dynamic_extent;
  test_construction<preview::extents<int>>();
  test_construction<preview::extents<unsigned, D>>();
  test_construction<preview::extents<unsigned, 7>>();
  test_construction<preview::extents<unsigned, 7, 8>>();
  test_construction<preview::extents<std::int64_t, D, 8, D, D>>();
}
