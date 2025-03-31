//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <mdspan>

// template<class OtherExtents>
//   constexpr explicit(extents_type::rank() > 0)
//     mapping(const layout_stride::mapping<OtherExtents>& other);
//
// Constraints: is_constructible_v<extents_type, OtherExtents> is true.
//
// Preconditions:
//   - If extents_type::rank() > 0 is true, then for all r in the range [0, extents_type::rank()),
//     other.stride(r) equals other.extents().fwd-prod-of-extents(r), and
//   - other.required_span_size() is representable as a value of type index_type ([basic.fundamental]).
//
// Effects: Direct-non-list-initializes extents_ with other.extents().

#include <array>
#include <cstddef>
#include <type_traits>

#include "preview/mdspan.h"
#include "preview/span.h" // dynamic_extent

#include "gtest.h"
#include "../print_to.h"

template <bool implicit, class ToE, class FromE>
constexpr void test_conversion(FromE src_exts) {
  using To   = preview::layout_left::mapping<ToE>;
  using From = preview::layout_stride::mapping<FromE>;
  std::array<typename FromE::index_type, FromE::rank()> strides;

  if (FromE::rank() > 0) {
    strides[0] = 1;
    for (size_t r = 1; r < FromE::rank(); r++)
      strides[r] = src_exts.extent(r - 1) * strides[r - 1];
  }
  From src(src_exts, strides);

  ASSERT_NOEXCEPT(To(src));
  To dest(src);
  ASSERT_EQ(dest, src);

#if PREVIEW_CXX_VERSION >= 17
  if constexpr (implicit) {
    To dest_implicit = src;
    ASSERT_EQ(dest_implicit, src);
  } else {
    ASSERT_FALSE((std::is_convertible<From, To>::value));
  }
#endif
}

template <class T1, class T2>
constexpr void test_conversion() {
  constexpr size_t D = preview::dynamic_extent;

  // clang-format off
  test_conversion<true,  preview::extents<T1>>(preview::extents<T2>());
  test_conversion<false, preview::extents<T1, D>>(preview::extents<T2, D>(5));
  test_conversion<false, preview::extents<T1, 5>>(preview::extents<T2, D>(5));
  test_conversion<false, preview::extents<T1, 5>>(preview::extents<T2, 5>());
  test_conversion<false, preview::extents<T1, 5, D>>(preview::extents<T2, D, D>(5, 5));
  test_conversion<false, preview::extents<T1, D, D>>(preview::extents<T2, D, D>(5, 5));
  test_conversion<false, preview::extents<T1, D, D>>(preview::extents<T2, D, 7>(5));
  test_conversion<false, preview::extents<T1, 5, 7>>(preview::extents<T2, 5, 7>());
  test_conversion<false, preview::extents<T1, 5, D, 8, D, D>>(preview::extents<T2, D, D, 8, 9, 1>(5, 7));
  test_conversion<false, preview::extents<T1, D, D, D, D, D>>(
                         preview::extents<T2, D, D, D, D, D>(5, 7, 8, 9, 1));
  test_conversion<false, preview::extents<T1, D, D, 8, 9, D>>(preview::extents<T2, D, 7, 8, 9, 1>(5));
  test_conversion<false, preview::extents<T1, 5, 7, 8, 9, 1>>(preview::extents<T2, 5, 7, 8, 9, 1>());
  // clang-format on
}

template <class IdxT, size_t... Extents>
using lr_mapping_t = preview::layout_right::mapping<preview::extents<IdxT, Extents...>>;
template <class IdxT, size_t... Extents>
using ls_mapping_t = preview::layout_stride::mapping<preview::extents<IdxT, Extents...>>;

constexpr void test_rank_mismatch() {
  constexpr size_t D = preview::dynamic_extent;

  static_assert(!std::is_constructible<lr_mapping_t<int, D>, ls_mapping_t<int>>::value, "");
  static_assert(!std::is_constructible<lr_mapping_t<int>, ls_mapping_t<int, D, D>>::value, "");
  static_assert(!std::is_constructible<lr_mapping_t<int, D>, ls_mapping_t<int, D, D>>::value, "");
  static_assert(!std::is_constructible<lr_mapping_t<int, D, D, D>, ls_mapping_t<int, D, D>>::value, "");
}

constexpr void test_static_extent_mismatch() {
  constexpr size_t D = preview::dynamic_extent;

  static_assert(!std::is_constructible<lr_mapping_t<int, D, 5>, ls_mapping_t<int, D, 4>>::value, "");
  static_assert(!std::is_constructible<lr_mapping_t<int, 5>, ls_mapping_t<int, 4>>::value, "");
  static_assert(!std::is_constructible<lr_mapping_t<int, 5, D>, ls_mapping_t<int, 4, D>>::value, "");
}

TEST(MdSpanLayoutLeft, VERSIONED(ctor_layout_stride)) {
  test_conversion<int, int>();
  test_conversion<int, size_t>();
  test_conversion<size_t, int>();
  test_conversion<size_t, long>();
  test_rank_mismatch();
  test_static_extent_mismatch();
}
