//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// <mdspan>

// constexpr mdspan(data_handle_type p, const mapping_type& m, const accessor_type& a);
//
// Preconditions: [0, m.required_span_size()) is an accessible range of p and a.
//
// Effects:
//   - Direct-non-list-initializes ptr_ with std::move(p),
//   - direct-non-list-initializes map_ with m, and
//   - direct-non-list-initializes acc_ with a.

#include <type_traits>

#include "preview/concepts.h"
#include "preview/core.h"
#include "preview/mdspan.h"
#include "preview/span.h"
#include "preview/type_traits.h"

#include "../../test_utils.h"

#include "../MinimalElementType.h"
#include "../CustomTestLayouts.h"
#include "CustomTestAccessors.h"

template <class H, class M, class A>
constexpr void test_mdspan_types(const H& handle, const M& map, const A& acc) {
  using MDS = preview::mdspan<typename A::element_type, typename M::extents_type, typename M::layout_type, A>;

  move_counted_handle<typename MDS::element_type>::move_counter() = 0;
  // use formulation of constructor which tests that it is not explicit
  MDS m = {handle, map, acc};

#if PREVIEW_CXX_VERSION >= 17
  if constexpr (std::is_same<H, move_counted_handle<typename MDS::element_type>>::value) {
    EXPECT_EQ(H::move_counter(), 1);
  }
#endif
  PREVIEW_STATIC_ASSERT(!noexcept(MDS(handle, map, acc)));

  EXPECT_EQ(m.extents(), map.extents());
#if PREVIEW_CXX_VERSION >= 17
  if constexpr (preview::equality_comparable<H>::value)
    EXPECT_EQ(m.data_handle(), handle);
  if constexpr (preview::equality_comparable<M>::value)
    EXPECT_EQ(m.mapping(), map);
  if constexpr (preview::equality_comparable<A>::value)
    EXPECT_EQ(m.accessor(), acc);
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
  mixin_extents(handle, preview::layout_left(), acc);
  mixin_extents(handle, preview::layout_right(), acc);
  mixin_extents(handle, layout_wrapping_integral<4>(), acc);
}

template <class T>
constexpr void mixin_accessor() {
  ElementPool<T, 1024> elements;
  mixin_layout(elements.get_ptr(), preview::default_accessor<T>());

  // Using weird accessor/data_handle
  // Make sure they actually got the properties we want to test
  // checked_accessor is not default constructible except for const double, where it is not noexcept
  PREVIEW_STATIC_ASSERT(std::is_default_constructible<checked_accessor<T>>::value == std::is_same<T, const double>::value);
  // checked_accessor's data handle type is not default constructible for double
  PREVIEW_STATIC_ASSERT(
      std::is_default_constructible<typename checked_accessor<T>::data_handle_type>::value != std::is_same<T, double>::value);
  mixin_layout(typename checked_accessor<T>::data_handle_type(elements.get_ptr()), checked_accessor<T>(1024));
}

template <class E>
using mapping_t = preview::layout_right::mapping<E>;

constexpr bool test() {
  mixin_accessor<int>();
  mixin_accessor<const int>();
  mixin_accessor<double>();
  mixin_accessor<const double>();
  mixin_accessor<MinimalElementType>();
  mixin_accessor<const MinimalElementType>();

  // test non-constructibility from wrong args
  constexpr size_t D = preview::dynamic_extent;
  using mds_t        = preview::mdspan<float, preview::extents<int, 3, D, D>>;
  using acc_t        = preview::default_accessor<float>;

  // sanity check
  PREVIEW_STATIC_ASSERT(std::is_constructible<mds_t, float*, mapping_t<preview::extents<int, 3, D, D>>, acc_t>::value);

  // test non-constructibility from wrong accessor
  PREVIEW_STATIC_ASSERT(
      !std::is_constructible<mds_t, float*, mapping_t<preview::extents<int, 3, D, D>>, preview::default_accessor<const float>>::value);

  // test non-constructibility from wrong mapping type
  // wrong rank
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_t, float*, mapping_t<preview::extents<int, D, D>>, acc_t>::value);
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_t, float*, mapping_t<preview::extents<int, D, D, D, D>>, acc_t>::value);
  // wrong type in general: note the map constructor does NOT convert, since it takes by const&
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_t, float*, mapping_t<preview::extents<int, D, D, D>>, acc_t>::value);
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_t, float*, mapping_t<preview::extents<unsigned, 3, D, D>>, acc_t>::value);

  // test non-constructibility from wrong handle_type
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_t, const float*, mapping_t<preview::extents<int, 3, D, D>>, acc_t>::value);

  return true;
}

int main(int, char**) {
  test();
  return 0;
}
