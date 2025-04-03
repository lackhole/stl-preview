//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// <mdspan>

// template<class OtherIndexType, size_t N>
//   constexpr explicit(N != rank_dynamic())
//     mdspan(data_handle_type p, const array<OtherIndexType, N>& exts);
//
// Constraints:
//   - is_convertible_v<const OtherIndexType&, index_type> is true,
//   - (is_nothrow_constructible<index_type, const OtherIndexType&> && ...) is true,
//   - N == rank() || N == rank_dynamic() is true,
//   - is_constructible_v<mapping_type, extents_type> is true, and
//   - is_default_constructible_v<accessor_type> is true.
//
// Preconditions: [0, map_.required_span_size()) is an accessible range of p and acc_
//                for the values of map_ and acc_ after the invocation of this constructor.
//
// Effects:
//   - Direct-non-list-initializes ptr_ with std::move(p),
//   - direct-non-list-initializes map_ with extents_type(exts), and
//   - value-initializes acc_.

#include <array>
#include <type_traits>

#include "preview/core.h"
#include "preview/mdspan.h"
#include "preview/span.h"
#include "preview/type_traits.h"

#include "../../test_utils.h"

#include "../ConvertibleToIntegral.h"
#include "../MinimalElementType.h"
#include "../CustomTestLayouts.h"
#include "CustomTestAccessors.h"

template <class Extents, size_t... Idxs>
constexpr auto array_from_extents(const Extents& exts, std::index_sequence<Idxs...>) {
  return std::array<typename Extents::index_type, Extents::rank()>{exts.extent(Idxs)...};
}

#if PREVIEW_CONFORM_CXX20_STANDARD
template <class MDS, class Exts>
concept check_mdspan_ctor_implicit = requires(MDS m, typename MDS::data_handle_type h, const Exts& exts) {
  m = {h, exts};
};
#endif

template <class H, class M, class A, size_t N>
constexpr void
test_mdspan_ctor_array(const H& handle, const M& map, const A&, std::array<typename M::index_type, N> exts) {
  using MDS = preview::mdspan<typename A::element_type, typename M::extents_type, typename M::layout_type, A>;
  move_counted_handle<typename MDS::element_type>::move_counter() = 0;
  MDS m(handle, exts);
  if PREVIEW_CONSTEXPR_AFTER_CXX17 (std::is_same<H, move_counted_handle<typename MDS::element_type>>::value) {
    EXPECT_EQ(H::move_counter(), 1);
  }

  PREVIEW_STATIC_ASSERT(!noexcept(MDS(handle, exts)));

#if PREVIEW_CONFORM_CXX20_STANDARD
  PREVIEW_STATIC_ASSERT(check_mdspan_ctor_implicit<MDS, decltype(exts)> == (N == MDS::rank_dynamic()));
#endif

  EXPECT_EQ(m.extents(), map.extents());
#if PREVIEW_CXX_VERSION >= 17
  if constexpr (preview::equality_comparable<H>::value)
    EXPECT_EQ(m.data_handle(), handle);
  if constexpr (preview::equality_comparable<M>::value)
    EXPECT_EQ(m.mapping(), map);
  if constexpr (preview::equality_comparable<A>::value)
    EXPECT_EQ(m.accessor(), A());
#endif
}

template <bool mec, bool ac, class H, class M, class A>
constexpr void test_mdspan_ctor(const H& handle, const M& map, const A& acc) {
  using MDS = preview::mdspan<typename A::element_type, typename M::extents_type, typename M::layout_type, A>;
  PREVIEW_STATIC_ASSERT(mec == std::is_constructible<M, typename M::extents_type>::value);
  PREVIEW_STATIC_ASSERT(ac == std::is_default_constructible<A>::value);

#if PREVIEW_CXX_VERSION >= 17
  if constexpr (mec && ac) {
    // test from all extents
    auto exts = array_from_extents(map.extents(), std::make_index_sequence<MDS::rank()>());
    test_mdspan_ctor_array(handle, map, acc, exts);

    // test from dynamic extents
    std::array<typename MDS::index_type, MDS::rank_dynamic()> exts_dynamic{};
    size_t r_dyn = 0;
    for (size_t r = 0; r < MDS::rank(); r++) {
      if (MDS::static_extent(r) == preview::dynamic_extent)
        exts_dynamic[r_dyn++] = exts[r];
    }
    test_mdspan_ctor_array(handle, map, acc, exts_dynamic);
  } else {
    PREVIEW_STATIC_ASSERT(!std::is_constructible<MDS, const H&, const std::array<typename MDS::index_type, MDS::rank()>&>::value);
  }
#endif
}

template <bool mec, bool ac, class H, class L, class A>
constexpr void mixin_extents(const H& handle, const L& layout, const A& acc) {
  constexpr size_t D = preview::dynamic_extent;
  test_mdspan_ctor<mec, ac>(handle, construct_mapping(layout, preview::extents<int>()), acc);
  test_mdspan_ctor<mec, ac>(handle, construct_mapping(layout, preview::extents<signed char, D>(7)), acc);
  test_mdspan_ctor<mec, ac>(handle, construct_mapping(layout, preview::extents<unsigned, 7>()), acc);
  test_mdspan_ctor<mec, ac>(handle, construct_mapping(layout, preview::extents<size_t, D, 4, D>(2, 3)), acc);
  test_mdspan_ctor<mec, ac>(handle, construct_mapping(layout, preview::extents<signed char, D, 7, D>(0, 3)), acc);
  test_mdspan_ctor<mec, ac>(
      handle, construct_mapping(layout, preview::extents<int64_t, D, 7, D, 4, D, D>(1, 2, 3, 2)), acc);
}

template <bool ac, class H, class A>
constexpr void mixin_layout(const H& handle, const A& acc) {
  mixin_extents<true, ac>(handle, preview::layout_left(), acc);
  mixin_extents<true, ac>(handle, preview::layout_right(), acc);

  // Sanity check that this layouts mapping is constructible from extents (via its move constructor)
  PREVIEW_STATIC_ASSERT(std::is_constructible<layout_wrapping_integral<8>::mapping<preview::extents<int>>, preview::extents<int>>::value);
  PREVIEW_STATIC_ASSERT(
      !std::is_constructible<layout_wrapping_integral<8>::mapping<preview::extents<int>>, const preview::extents<int>&>::value);
  mixin_extents<true, ac>(handle, layout_wrapping_integral<8>(), acc);
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

  // test non-constructibility from wrong array type
  constexpr size_t D = preview::dynamic_extent;
  using mds_t        = preview::mdspan<float, preview::extents<unsigned, 3, D, D>>;
  // sanity check
  PREVIEW_STATIC_ASSERT(std::is_constructible<mds_t, float*, std::array<int, 3>>::value);
  PREVIEW_STATIC_ASSERT(std::is_constructible<mds_t, float*, std::array<int, 2>>::value);
  // wrong size
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_t, float*, std::array<int, 1>>::value);
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_t, float*, std::array<int, 4>>::value);
  // not convertible to index_type
  PREVIEW_STATIC_ASSERT(std::is_convertible<const IntType&, int>::value);
  PREVIEW_STATIC_ASSERT(!std::is_convertible<const IntType&, unsigned>::value);
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_t, float*, std::array<IntType, 2>>::value);

  // index_type is not nothrow constructible
  using mds_uchar_t = preview::mdspan<float, preview::extents<unsigned char, 3, D, D>>;
  PREVIEW_STATIC_ASSERT(std::is_convertible<IntType, unsigned char>::value);
  PREVIEW_STATIC_ASSERT(std::is_convertible<const IntType&, unsigned char>::value);
  PREVIEW_STATIC_ASSERT(!std::is_nothrow_constructible<unsigned char, const IntType&>::value);
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_uchar_t, float*, std::array<IntType, 2>>::value);

  // convertible from non-const to index_type but not  from const
  using mds_int_t = preview::mdspan<float, preview::extents<int, 3, D, D>>;
  PREVIEW_STATIC_ASSERT(std::is_convertible<IntTypeNC, int>::value);
  PREVIEW_STATIC_ASSERT(!std::is_convertible<const IntTypeNC&, int>::value);
  PREVIEW_STATIC_ASSERT(std::is_nothrow_constructible<int, IntTypeNC>::value);
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_int_t, float*, std::array<IntTypeNC, 2>>::value);

  // can't test a combo where std::is_nothrow_constructible_v<int, const IntTypeNC&> is true,
  // but std::is_convertible_v<const IntType&, int> is false

  // test non-constructibility from wrong handle_type
  PREVIEW_STATIC_ASSERT(!std::is_constructible<mds_t, const float*, std::array<int, 2>>::value);

  return true;
}

int main(int, char**) {
  test();
  return 0;
}
