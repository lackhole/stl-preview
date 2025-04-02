//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// <mdspan>

// constexpr mdspan(data_handle_type p, const extents_type& ext);
//
// Constraints:
//   - is_constructible_v<mapping_type, const extents_type&> is true, and
//   - is_default_constructible_v<accessor_type> is true.
//
// Preconditions: [0, map_.required_span_size()) is an accessible range of p and acc_
//                for the values of map_ and acc_ after the invocation of this constructor.
//
// Effects:
//   - Direct-non-list-initializes ptr_ with std::move(p),
//   - direct-non-list-initializes map_ with ext, and
//   - value-initializes acc_.

#include <type_traits>

#include "preview/concepts.h"
#include "preview/core.h"
#include "preview/mdspan.h"
#include "preview/span.h"

#include "../../test_utils.h"

#include "../MinimalElementType.h"
#include "../CustomTestLayouts.h"
#include "CustomTestAccessors.h"

template <bool mec, bool ac, class H, class M, class A>
constexpr void test_mdspan_types(const H& handle, const M& map, const A&) {
  using MDS = preview::mdspan<typename A::element_type, typename M::extents_type, typename M::layout_type, A>;

  PREVIEW_STATIC_ASSERT(mec == std::is_constructible<M, const typename M::extents_type&>::value);
  PREVIEW_STATIC_ASSERT(ac == std::is_default_constructible<A>::value);

#if PREVIEW_CXX_VERSION >= 17
  if constexpr (mec && ac) {
    move_counted_handle<typename MDS::element_type>::move_counter() = 0;
    // use formulation of constructor which tests that its not explicit
    MDS m = {handle, map.extents()};
    if constexpr (std::is_same_v<H, move_counted_handle<typename MDS::element_type>>) {
      EXPECT_EQ(H::move_counter(), 1);
    }
    PREVIEW_STATIC_ASSERT(!noexcept(MDS(handle, map.extents())));
    EXPECT_EQ(m.extents(), map.extents());
    if constexpr (preview::equality_comparable<H>::value)
      EXPECT_EQ(m.data_handle(), handle);
    if constexpr (preview::equality_comparable<M>::value)
      EXPECT_EQ(m.mapping(), map);
    if constexpr (preview::equality_comparable<A>::value)
      EXPECT_EQ(m.accessor(), A());
  } else {
    PREVIEW_STATIC_ASSERT(!std::is_constructible<MDS, const H&, const typename M::extents_type&>::value);
  }
#endif
}

template <bool mec, bool ac, class H, class L, class A>
constexpr void mixin_extents(const H& handle, const L& layout, const A& acc) {
  constexpr size_t D = preview::dynamic_extent;
  test_mdspan_types<mec, ac>(handle, construct_mapping(layout, preview::extents<int>()), acc);
  test_mdspan_types<mec, ac>(handle, construct_mapping(layout, preview::extents<signed char, D>(7)), acc);
  test_mdspan_types<mec, ac>(handle, construct_mapping(layout, preview::extents<unsigned, 7>()), acc);
  test_mdspan_types<mec, ac>(handle, construct_mapping(layout, preview::extents<size_t, D, 4, D>(2, 3)), acc);
  test_mdspan_types<mec, ac>(handle, construct_mapping(layout, preview::extents<signed char, D, 7, D>(0, 3)), acc);
  test_mdspan_types<mec, ac>(
      handle, construct_mapping(layout, preview::extents<int64_t, D, 7, D, 4, D, D>(1, 2, 3, 2)), acc);
}

template <bool ac, class H, class A>
constexpr void mixin_layout(const H& handle, const A& acc) {
  mixin_extents<true, ac>(handle, preview::layout_left(), acc);
  mixin_extents<true, ac>(handle, preview::layout_right(), acc);

  // Use weird layout, make sure it has the properties we want to test
  // Sanity check that this layouts mapping is constructible from extents (via its move constructor)
  PREVIEW_STATIC_ASSERT(std::is_constructible<layout_wrapping_integral<8>::mapping<preview::extents<int>>, preview::extents<int>>::value);
  PREVIEW_STATIC_ASSERT(
      !std::is_constructible<layout_wrapping_integral<8>::mapping<preview::extents<int>>, const preview::extents<int>&>::value);
  mixin_extents<false, ac>(handle, layout_wrapping_integral<8>(), acc);
  // Sanity check that this layouts mapping is not constructible from extents
  PREVIEW_STATIC_ASSERT(!std::is_constructible<layout_wrapping_integral<4>::mapping<preview::extents<int>>, preview::extents<int>>::value);
  PREVIEW_STATIC_ASSERT(
      !std::is_constructible<layout_wrapping_integral<4>::mapping<preview::extents<int>>, const preview::extents<int>&>::value);
  mixin_extents<false, ac>(handle, layout_wrapping_integral<4>(), acc);
}

template <class T>
constexpr void mixin_accessor() {
  ElementPool<T, 1024> elements;
  mixin_layout<true>(elements.get_ptr(), preview::default_accessor<T>());

  // Using weird accessor/data_handle
  // Make sure they actually got the properties we want to test
  // checked_accessor is not default constructible except for const double, where it is not noexcept
  PREVIEW_STATIC_ASSERT(std::is_default_constructible<checked_accessor<T>>::value == std::is_same<T, const double>::value);
  mixin_layout<std::is_same<T, const double>::value>(
      typename checked_accessor<T>::data_handle_type(elements.get_ptr()), checked_accessor<T>(1024));
}

constexpr bool test() {
  mixin_accessor<int>();
  mixin_accessor<const int>();
  mixin_accessor<double>();
  mixin_accessor<const double>();
  mixin_accessor<MinimalElementType>();
  mixin_accessor<const MinimalElementType>();

  // test non-constructibility from wrong extents type
  constexpr size_t D = preview::dynamic_extent;
  using mds_t        = preview::mdspan<float, preview::extents<int, 3, D, D>>;
  // sanity check
  PREVIEW_STATIC_ASSERT(std::is_constructible<mds_t, float*, preview::extents<int, 3, D, D>>::value);
  // wrong size
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_t, float*, preview::extents<int, D, D>>::value);
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_t, float*, preview::extents<int, D, D, D, D>>::value);
  // wrong type in general: note the extents constructor does NOT convert, since it takes by const&
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_t, float*, preview::extents<int, D, D, D>>::value);
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_t, float*, preview::extents<unsigned, 3, D, D>>::value);

  // test non-constructibility from wrong handle_type
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_t, const float*, preview::extents<int, 3, D, D>>::value);

  return true;
}
int main(int, char**) {
  test();
  return 0;
}
