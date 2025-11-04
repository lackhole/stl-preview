//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// <mdspan>

// constexpr mdspan();
// Constraints:
//   - rank_dynamic() > 0 is true.
//   - is_default_constructible_v<data_handle_type> is true.
//   - is_default_constructible_v<mapping_type> is true.
//   - is_default_constructible_v<accessor_type> is true.
//
// Preconditions: [0, map_.required_span_size()) is an accessible range of ptr_
//               and acc_ for the values of map_ and acc_ after the invocation of this constructor.
//
// Effects: Value-initializes ptr_, map_, and acc_.

#include "preview/mdspan.h"

#include <type_traits>

#include "preview/core.h"
#include "preview/concepts.h"
#include "preview/span.h"
#include "../../test_utils.h"

#include "../MinimalElementType.h"
#include "../CustomTestLayouts.h"
#include "CustomTestAccessors.h"

template <bool hc, bool mc, bool ac, class H, class M, class A>
constexpr void test_mdspan_types(const H&, const M&, const A&) {
  using MDS = preview::mdspan<typename A::element_type, typename M::extents_type, typename M::layout_type, A>;

  PREVIEW_STATIC_ASSERT(hc == std::is_default_constructible<H>::value);
  PREVIEW_STATIC_ASSERT(mc == std::is_default_constructible<M>::value);
  PREVIEW_STATIC_ASSERT(ac == std::is_default_constructible<A>::value);

#if PREVIEW_CXX_VERSION >= 17
  if constexpr (MDS::rank_dynamic() > 0 && hc && mc && ac) {
    MDS m;
    static_assert(noexcept(MDS()) == (noexcept(H()) && noexcept(M()) && noexcept(A())));
    EXPECT_EQ(m.extents(), typename MDS::extents_type());
    if constexpr (preview::equality_comparable<H>::value)
      EXPECT_EQ(m.data_handle(), H());
    if constexpr (preview::equality_comparable<M>::value) {
      EXPECT_EQ(m.mapping(), M());
      EXPECT_EQ(M(), m.mapping());
    }
    if constexpr (preview::equality_comparable<A>::value)
      EXPECT_EQ(m.accessor(), A());
  } else {
    static_assert(!std::is_default_constructible<MDS>::value);
  }
#endif
}

template <bool hc, bool mc, bool ac, class H, class L, class A>
constexpr void mixin_extents(const H& handle, const L& layout, const A& acc) {
  constexpr size_t D = preview::dynamic_extent;
  test_mdspan_types<hc, mc, ac>(handle, construct_mapping(layout, preview::extents<int>()), acc);
  test_mdspan_types<hc, mc, ac>(handle, construct_mapping(layout, preview::extents<signed char, D>(7)), acc);
  test_mdspan_types<hc, mc, ac>(handle, construct_mapping(layout, preview::extents<unsigned, 7>()), acc);
  test_mdspan_types<hc, mc, ac>(handle, construct_mapping(layout, preview::extents<size_t, D, 4, D>(2, 3)), acc);
  test_mdspan_types<hc, mc, ac>(handle, construct_mapping(layout, preview::extents<signed char, D, 7, D>(0, 3)), acc);
  test_mdspan_types<hc, mc, ac>(
      handle, construct_mapping(layout, preview::extents<int64_t, D, 7, D, 4, D, D>(1, 2, 3, 2)), acc);
}

template <bool hc, bool ac, class H, class A>
constexpr void mixin_layout(const H& handle, const A& acc) {
  mixin_extents<hc, true, ac>(handle, preview::layout_left(), acc);
  mixin_extents<hc, true, ac>(handle, preview::layout_right(), acc);

  // Use weird layout, make sure it has the properties we want to test
  constexpr size_t D = preview::dynamic_extent;
  PREVIEW_STATIC_ASSERT(!std::is_default_constructible< layout_wrapping_integral<4>::mapping<preview::extents<signed char, D>>>::value);
  mixin_extents<hc, false, ac>(handle, layout_wrapping_integral<4>(), acc);
}

template <class T>
constexpr void mixin_accessor() {
  ElementPool<T, 1024> elements;
  mixin_layout<true, true>(elements.get_ptr(), preview::default_accessor<T>());

  // Using weird accessor/data_handle
  // Make sure they actually got the properties we want to test
  // checked_accessor is not default constructible except for const double, where it is not noexcept
  PREVIEW_STATIC_ASSERT(std::is_default_constructible<checked_accessor<T>>::value == std::is_same<T, const double>::value);
  // checked_accessor's data handle type is not default constructible for double
  PREVIEW_STATIC_ASSERT(
      std::is_default_constructible<typename checked_accessor<T>::data_handle_type>::value != std::is_same<T, double>::value);
  mixin_layout<!std::is_same<T, double>::value, std::is_same<T, const double>::value>(
      typename checked_accessor<T>::data_handle_type(elements.get_ptr()), checked_accessor<T>(1024));
}

TEST(MDSpan, VERSIONED(ctor_default)) {
  mixin_accessor<int>();
  mixin_accessor<const int>();
  mixin_accessor<double>();
  mixin_accessor<const double>();
  mixin_accessor<MinimalElementType>();
  mixin_accessor<const MinimalElementType>();
}
