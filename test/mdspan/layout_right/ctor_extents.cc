//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <mdspan>

// constexpr mapping(const extents_type&) noexcept;
//
// Preconditions: The size of the multidimensional index space e is representable
//                as a value of type index_type ([basic.fundamental]).
//
// Effects: Direct-non-list-initializes extents_ with e.

#include <cstddef>
#include <cstdint>

#include "preview/mdspan.h"
#include "preview/span.h" // dynamic_extent

#include "gtest.h"
#include "../print_to.h"

template <class E>
constexpr void test_construction(E e) {
  using M = preview::layout_right::mapping<E>;
  ASSERT_NOEXCEPT(M{e});
  M m(e);

  // check correct extents are returned
  ASSERT_NOEXCEPT(m.extents());
  ASSERT_EQ(m.extents(), e);

  // check required_span_size()
  typename E::index_type expected_size = 1;
  for (typename E::rank_type r = 0; r < E::rank(); r++)
    expected_size *= e.extent(r);
  ASSERT_EQ(m.required_span_size(), expected_size);
}

TEST(MdSpanLayoutRight, VERSIONED(ctor_extents)) {
  constexpr size_t D = preview::dynamic_extent;
  test_construction(preview::extents<int>());
  test_construction(preview::extents<unsigned, D>(7));
  test_construction(preview::extents<unsigned, 7>());
  test_construction(preview::extents<unsigned, 7, 8>());
  test_construction(preview::extents<int64_t, D, 8, D, D>(7, 9, 10));
}
