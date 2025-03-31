//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// <mdspan>

// constexpr mdspan(const mdspan&) = default;
//
// A specialization of mdspan is a trivially copyable type if its accessor_type, mapping_type, and data_handle_type are trivially copyable types.

#include "preview/mdspan.h"

#include <type_traits>

#include "preview/concepts.h"
#include "preview/span.h"

#include "gtest.h"

#include "../MinimalElementType.h"
#include "../CustomTestLayouts.h"
#include "CustomTestAccessors.h"

template <class H, class M, class A>
constexpr void test_mdspan_types(const H& handle, const M& map, const A& acc) {
  using MDS = preview::mdspan<typename A::element_type, typename M::extents_type, typename M::layout_type, A>;

  MDS m_org(handle, map, acc);
  MDS m(m_org);
  static_assert(noexcept(MDS(m_org)) == (noexcept(H(handle))&& noexcept(M(map))&& noexcept(A(acc))), "");
  static_assert(
      std::is_trivially_copyable<MDS>::value ==
      (std::is_trivially_copyable<H>::value && std::is_trivially_copyable<M>::value && std::is_trivially_copyable<A>::value), "");
  ASSERT_EQ(m.extents(), map.extents());

#if PREVIEW_CXX_VERSION >= 17
  if constexpr (preview::equality_comparable<H>::value)
    ASSERT_EQ(m.data_handle(), handle);
  if constexpr (preview::equality_comparable<M>::value)
    ASSERT_EQ(m.mapping(), map);
  if constexpr (preview::equality_comparable<A>::value)
    ASSERT_EQ(m.accessor(), acc);
#endif
}

template <class H, class L, class A>
constexpr void mixin_extents(const H& handle, const L& layout, const A& acc) {
  constexpr size_t D = preview::dynamic_extent;
  test_mdspan_types(handle, construct_mapping(layout, preview::extents<int>()), acc);
  test_mdspan_types(handle, construct_mapping(layout, preview::extents<signed char, D>(7)), acc);
  test_mdspan_types(handle, construct_mapping(layout, preview::extents<unsigned, 7>()), acc);
  test_mdspan_types(handle, construct_mapping(layout, preview::extents<size_t, D, 4, D>(2, 3)), acc);
  test_mdspan_types(handle, construct_mapping(layout, preview::extents<signed char, D, 7, D>(0, 3)), acc);
  test_mdspan_types(handle, construct_mapping(layout, preview::extents<int64_t, D, 7, D, 4, D, D>(1, 2, 3, 2)), acc);
}

template <class H, class A>
constexpr void mixin_layout(const H& handle, const A& acc) {
  // make sure we test a trivially copyable mapping
  static_assert(std::is_trivially_copyable<preview::layout_left::mapping<preview::extents<int>>>::value, "");
  mixin_extents(handle, preview::layout_left(), acc);
  mixin_extents(handle, preview::layout_right(), acc);
  // make sure we test a not trivially copyable mapping
  static_assert(!std::is_trivially_copyable<layout_wrapping_integral<4>::mapping<preview::extents<int>>>::value, "");
  mixin_extents(handle, layout_wrapping_integral<4>(), acc);
}

template <class T>
constexpr void mixin_accessor() {
  ElementPool<T, 1024> elements;
  // make sure we test trivially constructible accessor and data_handle
  static_assert(std::is_trivially_copyable<preview::default_accessor<T>>::value, "");
  static_assert(std::is_trivially_copyable<typename preview::default_accessor<T>::data_handle_type>::value, "");
  mixin_layout(elements.get_ptr(), preview::default_accessor<T>());

  // Using weird accessor/data_handle
  // Make sure they actually got the properties we want to test
  // checked_accessor is noexcept copy constructible except for const double
  checked_accessor<T> acc(1024);
  static_assert(noexcept(checked_accessor<T>(acc)) != std::is_same<T, const double>::value, "");
  mixin_layout(typename checked_accessor<T>::data_handle_type(elements.get_ptr()), acc);
}

TEST(MDSpan, VERSIONED(ctor_copy)) {
  mixin_accessor<int>();
  mixin_accessor<const int>();
  mixin_accessor<double>();
  mixin_accessor<const double>();
  mixin_accessor<MinimalElementType>();
  mixin_accessor<const MinimalElementType>();
}
