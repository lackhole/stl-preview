//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// <mdspan>
//
// friend constexpr void swap(mdspan& x, mdspan& y) noexcept;
//
// Effects: Equivalent to:
//   swap(x.ptr_, y.ptr_);
//   swap(x.map_, y.map_);
//   swap(x.acc_, y.acc_);

#include <type_traits>

#include "preview/concepts.h"
#include "preview/core.h"
#include "preview/mdspan.h"
#include "preview/span.h"

#include "../../test_utils.h"

#include "../MinimalElementType.h"
#include "../CustomTestLayouts.h"

template <class MDS>
constexpr void test_swap(MDS a, MDS b) {
  auto org_a = a;
  auto org_b = b;
  swap(a, b);
  EXPECT_EQ(a.extents(), org_b.extents());
  EXPECT_EQ(b.extents(), org_a.extents());

#if PREVIEW_CXX_VERSION >= 17
  if constexpr (preview::equality_comparable<typename MDS::mapping_type>::value) {
    EXPECT_EQ(a.mapping(), org_b.mapping());
    EXPECT_EQ(b.mapping(), org_a.mapping());
  }
  if constexpr (preview::equality_comparable<typename MDS::data_handle_type>::value) {
    EXPECT_EQ(a.data_handle(), org_b.data_handle());
    EXPECT_EQ(b.data_handle(), org_a.data_handle());
  }
  // This check uses a side effect of layout_wrapping_integral::swap to make sure
  // mdspan calls the underlying components' swap via ADL
  if constexpr (std::is_same_v<typename MDS::layout_type, layout_wrapping_integral<4>>) {
    EXPECT_GE(MDS::mapping_type::swap_counter(), 0);
  }
#endif
}

constexpr bool test() {
#if PREVIEW_CXX_VERSION >= 17
  using extents_t = preview::extents<int, 4, preview::dynamic_extent>;
  float data_a[1024]{};
  float data_b[1024]{};
  {
    preview::mdspan a(data_a, extents_t(12));
    preview::mdspan b(data_b, extents_t(5));
    test_swap(a, b);
  }
  {
    layout_wrapping_integral<4>::mapping<extents_t> map_a(extents_t(12), not_extents_constructible_tag()),
        map_b(extents_t(5), not_extents_constructible_tag());
    preview::mdspan a(data_a, map_a);
    preview::mdspan b(data_b, map_b);
    test_swap(a, b);
  }
#endif
  return true;
}

int main(int, char**) {
  test();
  return 0;
}
