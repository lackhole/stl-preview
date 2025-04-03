//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <mdspan>

// constexpr index_type required_span_size() const noexcept;
//
// Returns: extents().fwd-prod-of-extents(extents_type::rank()).

#include <cstddef>
#include <cstdint>

#include "preview/mdspan.h"
#include "preview/span.h" // dynamic_extent

#include "../../test_utils.h"
#include "../print_to.h"

template <class E>
constexpr void test_required_span_size(E e, typename E::index_type expected_size) {
  using M = preview::layout_right::mapping<E>;
  const M m(e);

  ASSERT_NOEXCEPT(m.required_span_size());
  EXPECT_EQ(m.required_span_size(), expected_size);
}

TEST(MdspanLayoutRight, VERSIONED(required_span_size)) {
  constexpr size_t D = preview::dynamic_extent;
  test_required_span_size(preview::extents<int>(), 1);
  test_required_span_size(preview::extents<unsigned, D>(0), 0);
  test_required_span_size(preview::extents<unsigned, D>(1), 1);
  test_required_span_size(preview::extents<unsigned, D>(7), 7);
  test_required_span_size(preview::extents<unsigned, 7>(), 7);
  test_required_span_size(preview::extents<unsigned, 7, 8>(), 56);
  test_required_span_size(preview::extents<int64_t, D, 8, D, D>(7, 9, 10), 5040);
  test_required_span_size(preview::extents<int64_t, 1, 8, D, D>(9, 10), 720);
  test_required_span_size(preview::extents<int64_t, 1, 0, D, D>(9, 10), 0);
}
