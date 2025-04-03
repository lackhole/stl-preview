//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// MSVC warning C4244: 'initializing': conversion from '_Ty' to '_Ty', possible loss of data
// ADDITIONAL_COMPILE_FLAGS(cl-style-warnings): /wd4244

// <mdspan>

// template<class OtherElementType, class OtherExtents,
//         class OtherLayoutPolicy, class OtherAccessor>
//  constexpr explicit(see below)
//    mdspan(const mdspan<OtherElementType, OtherExtents,
//                        OtherLayoutPolicy, OtherAccessor>& other);
//
// Constraints:
//   - is_constructible_v<mapping_type, const OtherLayoutPolicy::template mapping<OtherExtents>&> is true, and
//   - is_constructible_v<accessor_type, const OtherAccessor&> is true.
// Mandates:
//   - is_constructible_v<data_handle_type, const OtherAccessor::data_handle_type&> is
//   - is_constructible_v<extents_type, OtherExtents> is true.
//
// Preconditions:
//   - For each rank index r of extents_type, static_extent(r) == dynamic_extent || static_extent(r) == other.extent(r) is true.
//   - [0, map_.required_span_size()) is an accessible range of ptr_ and acc_ for values of ptr_, map_, and acc_ after the invocation of this constructor.
//
// Effects:
//   - Direct-non-list-initializes ptr_ with other.ptr_,
//   - direct-non-list-initializes map_ with other.map_, and
//   - direct-non-list-initializes acc_ with other.acc_.
//
// Remarks: The expression inside explicit is equivalent to:
//   !is_convertible_v<const OtherLayoutPolicy::template mapping<OtherExtents>&, mapping_type>
//   || !is_convertible_v<const OtherAccessor&, accessor_type>

#include "preview/mdspan.h"

#include <type_traits>

#include "preview/concepts.h"
#include "preview/core.h"
#include "preview/span.h"

#include "../../test_utils.h"

#include "../MinimalElementType.h"
#include "../CustomTestLayouts.h"
#include "CustomTestAccessors.h"

template <class ToMDS, class FromMDS>
constexpr void test_implicit_conversion(ToMDS to_mds, FromMDS from_mds) {
  EXPECT_EQ(to_mds.extents(), from_mds.extents());
  EXPECT_EQ(from_mds.extents(), to_mds.extents());
#if PREVIEW_CXX_VERSION >= 17
  if constexpr (preview::equality_comparable_with<typename ToMDS::data_handle_type, typename FromMDS::data_handle_type>::value) {
    EXPECT_EQ(to_mds.data_handle(), from_mds.data_handle());
    EXPECT_EQ(from_mds.data_handle(), to_mds.data_handle());
  }
  if constexpr (preview::equality_comparable_with<typename ToMDS::mapping_type, typename FromMDS::mapping_type>::value) {
    EXPECT_EQ(to_mds.mapping(), from_mds.mapping());
    EXPECT_EQ(from_mds.mapping(), to_mds.mapping());
  }
  if constexpr (preview::equality_comparable_with<typename ToMDS::accessor_type, typename FromMDS::accessor_type>::value) {
    EXPECT_EQ(to_mds.accessor(), from_mds.accessor());
    EXPECT_EQ(from_mds.accessor(), to_mds.accessor());
  }
#endif
}

template<typename M, bool = preview::conjunction_v<preview::copyable<M>, preview::equality_comparable<M>>>
struct mapping_requirements : std::false_type {};
template<typename M>
struct mapping_requirements<M, true>
    : preview::conjunction<
        std::is_nothrow_move_constructible<M>,
        std::is_nothrow_move_assignable<M>,
        preview::is_nothrow_swappable<M>
    > {};

template <typename ToMDS, typename FromMDS>
constexpr void test_conversion(FromMDS from_mds) {
  // check some requirements, to see we didn't screw up our test layouts/accessors
  static_assert(mapping_requirements<typename ToMDS::mapping_type>::value, "");
  static_assert(mapping_requirements<typename FromMDS::mapping_type>::value, "");

  constexpr bool constructible =
      std::is_constructible<typename ToMDS::mapping_type, const typename FromMDS::mapping_type&>::value &&
      std::is_constructible<typename ToMDS::accessor_type, const typename FromMDS::accessor_type&>::value;
  constexpr bool convertible =
      std::is_convertible<const typename FromMDS::mapping_type&, typename ToMDS::mapping_type>::value &&
      std::is_convertible<const typename FromMDS::accessor_type&, typename ToMDS::accessor_type>::value;
  constexpr bool passes_mandates =
      std::is_constructible<typename ToMDS::data_handle_type, const typename FromMDS::data_handle_type&>::value &&
      std::is_constructible<typename ToMDS::extents_type, typename FromMDS::extents_type>::value;

  if PREVIEW_CONSTEXPR_AFTER_CXX17 (constructible) {
    if PREVIEW_CONSTEXPR_AFTER_CXX17 (passes_mandates) {
      ToMDS to_mds(from_mds);
      EXPECT_EQ(to_mds.extents(), from_mds.extents());
      EXPECT_EQ(from_mds.extents(), to_mds.extents());

      if PREVIEW_CONSTEXPR_AFTER_CXX17 (preview::equality_comparable_with<typename ToMDS::data_handle_type, typename FromMDS::data_handle_type>::value) {
        EXPECT_EQ(to_mds.data_handle(), from_mds.data_handle());
        EXPECT_EQ(from_mds.data_handle(), to_mds.data_handle());
      }
      if PREVIEW_CONSTEXPR_AFTER_CXX17 (preview::equality_comparable_with<typename ToMDS::mapping_type, typename FromMDS::mapping_type>::value) {
        EXPECT_EQ(to_mds.mapping(), from_mds.mapping());
        EXPECT_EQ(from_mds.mapping(), to_mds.mapping());
      }
      if PREVIEW_CONSTEXPR_AFTER_CXX17 (preview::equality_comparable_with<typename ToMDS::accessor_type, typename FromMDS::accessor_type>::value) {
        EXPECT_EQ(to_mds.accessor(), from_mds.accessor());
        EXPECT_EQ(from_mds.accessor(), to_mds.accessor());
      }
      if PREVIEW_CONSTEXPR_AFTER_CXX17 (convertible) {
        test_implicit_conversion(from_mds, from_mds);
      } else {
        PREVIEW_STATIC_ASSERT(!std::is_convertible<FromMDS, ToMDS>::value);
      }
    }
  } else {
    PREVIEW_STATIC_ASSERT(!std::is_constructible<ToMDS, FromMDS>::value);
  }
}

template <class ToL, class ToE, class ToA, class FromH, class FromL, class FromE, class FromA>
constexpr void construct_from_mds(const FromH& handle, const FromL& layout, const FromE& exts, const FromA& acc) {
  using ToMDS   = preview::mdspan<typename ToA::element_type, ToE, ToL, ToA>;
  using FromMDS = preview::mdspan<typename FromA::element_type, FromE, FromL, FromA>;
  test_conversion<ToMDS>(FromMDS(handle, construct_mapping(layout, exts), acc));
}

template <class ToL, class ToA, class FromH, class FromL, class FromA>
constexpr void mixin_extents(const FromH& handle, const FromL& layout, const FromA& acc) {
  constexpr size_t D = preview::dynamic_extent;
  // constructible and convertible
  construct_from_mds<ToL, preview::dextents<int, 0>, ToA>(handle, layout, preview::dextents<int, 0>(), acc);
  construct_from_mds<ToL, preview::dextents<int, 1>, ToA>(handle, layout, preview::dextents<int, 1>(4), acc);
  construct_from_mds<ToL, preview::dextents<int, 1>, ToA>(handle, layout, preview::extents<int, 4>(), acc);
  construct_from_mds<ToL, preview::dextents<int, 2>, ToA>(handle, layout, preview::dextents<int, 2>(4, 5), acc);
  construct_from_mds<ToL, preview::dextents<unsigned, 2>, ToA>(handle, layout, preview::dextents<int, 2>(4, 5), acc);
  construct_from_mds<ToL, preview::dextents<unsigned, 2>, ToA>(handle, layout, preview::extents<int, D, 5>(4), acc);
  construct_from_mds<ToL, preview::extents<int, D, 5>, ToA>(handle, layout, preview::extents<int, D, 5>(4), acc);
  construct_from_mds<ToL, preview::extents<int, D, 5>, ToA>(handle, layout, preview::extents<int, D, 5>(4), acc);
  construct_from_mds<ToL, preview::extents<int, D, 5, D, 7>, ToA>(handle, layout, preview::extents<int, D, 5, D, 7>(4, 6), acc);

  // not convertible
  construct_from_mds<ToL, preview::dextents<int, 1>, ToA>(handle, layout, preview::dextents<unsigned, 1>(4), acc);
  construct_from_mds<ToL, preview::extents<int, D, 5, D, 7>, ToA>(
      handle, layout, preview::extents<int, D, 5, D, D>(4, 6, 7), acc);

  // not constructible
  construct_from_mds<ToL, preview::dextents<int, 1>, ToA>(handle, layout, preview::dextents<int, 2>(4, 5), acc);
  construct_from_mds<ToL, preview::extents<int, D, 5, D, 8>, ToA>(handle, layout, preview::extents<int, D, 5, D, 7>(4, 6), acc);
}

template <class ToA, class FromH, class FromA>
constexpr void mixin_layout(const FromH& handle, const FromA& acc) {
  mixin_extents<preview::layout_left, ToA>(handle, preview::layout_left(), acc);
  mixin_extents<preview::layout_right, ToA>(handle, preview::layout_right(), acc);
  // Check layout policy conversion
  // different layout policies, but constructible and convertible
  PREVIEW_STATIC_ASSERT(std::is_constructible<preview::layout_left::mapping<preview::dextents<int, 1>>,
                                      const preview::layout_right::mapping<preview::dextents<int, 1>>&>::value);
  PREVIEW_STATIC_ASSERT(std::is_convertible<const preview::layout_right::mapping<preview::dextents<int, 1>>&,
                                    preview::layout_left::mapping<preview::dextents<int, 1>>>::value);
  // different layout policies, not constructible
  PREVIEW_STATIC_ASSERT(!std::is_constructible<preview::layout_left::mapping<preview::dextents<int, 2>>,
                                       const preview::layout_right::mapping<preview::dextents<int, 2>>&>::value);
  // different layout policies, constructible and not convertible
  PREVIEW_STATIC_ASSERT(std::is_constructible<preview::layout_left::mapping<preview::dextents<int, 1>>,
                                      const preview::layout_right::mapping<preview::dextents<size_t, 1>>&>::value);
  PREVIEW_STATIC_ASSERT(!std::is_convertible<const preview::layout_right::mapping<preview::dextents<size_t, 1>>&,
                                     preview::layout_left::mapping<preview::dextents<int, 1>>>::value);

  mixin_extents<preview::layout_left, ToA>(handle, preview::layout_right(), acc);
  mixin_extents<layout_wrapping_integral<4>, ToA>(handle, layout_wrapping_integral<4>(), acc);
  // different layout policies, constructible and not convertible
  PREVIEW_STATIC_ASSERT(!std::is_constructible<layout_wrapping_integral<8>::mapping<preview::dextents<unsigned, 2>>,
                                       const layout_wrapping_integral<8>::mapping<preview::dextents<int, 2>>&>::value);

  PREVIEW_STATIC_ASSERT(std::is_constructible<layout_wrapping_integral<8>::mapping<preview::dextents<unsigned, 2>>,
                                      layout_wrapping_integral<8>::mapping<preview::dextents<int, 2>>>::value);
  mixin_extents<layout_wrapping_integral<8>, ToA>(handle, layout_wrapping_integral<8>(), acc);
}

// check that we cover all corners with respect to constructibility and convertibility
template <bool constructible_constref_acc,
          bool convertible_constref_acc,
          bool constructible_nonconst_acc,
          bool convertible_nonconst_acc,
          bool constructible_constref_handle,
          bool convertible_constref_handle,
          bool constructible_nonconst_handle,
          bool convertible_nonconst_handle,
          class ToA,
          class FromA>
constexpr bool test(FromA from_acc) {
  PREVIEW_STATIC_ASSERT(preview::copyable<ToA>::value);
  PREVIEW_STATIC_ASSERT(preview::copyable<FromA>::value);
  PREVIEW_STATIC_ASSERT(std::is_constructible<ToA, const FromA&>::value == constructible_constref_acc);
  PREVIEW_STATIC_ASSERT(std::is_constructible<ToA, FromA>::value == constructible_nonconst_acc);
  PREVIEW_STATIC_ASSERT(std::is_constructible<typename ToA::data_handle_type, const typename FromA::data_handle_type&>::value == constructible_constref_handle);
  PREVIEW_STATIC_ASSERT(std::is_constructible<typename ToA::data_handle_type, typename FromA::data_handle_type>::value == constructible_nonconst_handle);
  PREVIEW_STATIC_ASSERT(std::is_convertible<const FromA&, ToA>::value == convertible_constref_acc);
  PREVIEW_STATIC_ASSERT(std::is_convertible<FromA, ToA>::value == convertible_nonconst_acc);
  PREVIEW_STATIC_ASSERT(std::is_convertible<const typename FromA::data_handle_type&, typename ToA::data_handle_type>::value == convertible_constref_handle);
  PREVIEW_STATIC_ASSERT(std::is_convertible<typename FromA::data_handle_type, typename ToA::data_handle_type>::value == convertible_nonconst_handle);

  ElementPool<typename FromA::element_type, 1024> elements;
  mixin_layout<ToA>(typename FromA::data_handle_type(elements.get_ptr()), from_acc);
  return true;
}

#if PREVIEW_CXX_VERSION >= 17

TEST(MDSpan, VERSIONED(conversion)) {
  // using shorthands here: t and o for better visual distinguishability
  constexpr bool t = true;
  constexpr bool o = false;

  // possibility matrix for constructibility and convertibility https://godbolt.org/z/98KGo8Wbc
  // you can't have convertibility without constructibility
  // and if you take const T& then you also can take T
  // this leaves 7 combinations
  // const_ref_ctor, const_ref_conv, nonconst_ctor, nonconst_conv, tested
  // o o o o X
  // o o t o X
  // o o t t X
  // t o t o X
  // t o t t X
  // t t t o X
  // t t t t X

  // checked_accessor has various weird data handles and some weird conversion properties
  // conv_test_accessor_c/nc is an accessor pair which has configurable conversion properties, but plain ptr as data handle
  // accessor constructible
  test<t, t, t, t, t, t, t, t, preview::default_accessor<float>>(preview::default_accessor<float>());
  test<t, t, t, t, t, t, t, t, preview::default_accessor<const float>>(preview::default_accessor<float>());
  test<t, t, t, t, t, t, t, t, preview::default_accessor<MinimalElementType>>(preview::default_accessor<MinimalElementType>());
  test<t, t, t, t, t, t, t, t, preview::default_accessor<const MinimalElementType>>(
      preview::default_accessor<MinimalElementType>());
  test<t, t, t, t, t, t, t, t, checked_accessor<int>>(checked_accessor<int>(1024));
  test<t, o, t, o, t, t, t, t, checked_accessor<const int>>(checked_accessor<int>(1024));
  test<t, t, t, t, o, o, o, o, checked_accessor<const unsigned>>(checked_accessor<unsigned>(1024));
  test<t, t, t, t, t, t, t, t, checked_accessor<float>>(checked_accessor<float>(1024));
  test<t, t, t, t, t, t, t, t, checked_accessor<double>>(checked_accessor<double>(1024));
  test<t, t, t, t, t, t, t, t, checked_accessor<MinimalElementType>>(checked_accessor<MinimalElementType>(1024));
  test<t, o, t, o, t, t, t, t, checked_accessor<const MinimalElementType>>(checked_accessor<MinimalElementType>(1024));
  test<t, o, t, o, t, t, t, t, conv_test_accessor_c<int, t, t, t, t>>(conv_test_accessor_nc<int, t, t>());
  test<t, o, t, t, t, t, t, t, conv_test_accessor_c<int, t, t, o, o>>(conv_test_accessor_nc<int, t, o>());
  // FIXME: these tests trigger what appears to be a compiler bug on MINGW32 with --target=x86_64-w64-windows-gnu
  // https://godbolt.org/z/KK8aj5bs7
  // Bug report: https://github.com/llvm/llvm-project/issues/64077
  #ifndef __MINGW32__
  test<t, t, t, o, t, t, t, t, conv_test_accessor_c<int, o, t, t, t>>(conv_test_accessor_nc<int, t, t>());
  test<t, t, t, t, t, t, t, t, conv_test_accessor_c<int, o, o, o, o>>(conv_test_accessor_nc<int, t, o>());
  #endif

  // ElementType convertible, but accessor not constructible
  test<o, o, o, o, o, o, o, o, preview::default_accessor<float>>(preview::default_accessor<int>());
  test<o, o, o, o, o, o, o, o, checked_accessor<const double>>(checked_accessor<double>(1024));
  test<o, o, t, t, t, t, t, t, checked_accessor<const float>>(checked_accessor<float>(1024));
  test<o, o, o, o, t, t, t, t, conv_test_accessor_c<int, o, o, t, t>>(conv_test_accessor_nc<int, o, o>());
  test<o, o, t, o, t, t, t, t, conv_test_accessor_c<int, o, t, o, o>>(conv_test_accessor_nc<int, o, t>());
  test<o, o, t, t, t, t, t, t, conv_test_accessor_c<int, o, o, t, t>>(conv_test_accessor_nc<int, o, t>());

  // Ran into trouble with doing it all in one static_assert: exceeding step limit for consteval
//  static_assert(test<t, t, t, t, t, t, t, t, preview::default_accessor<float>>(preview::default_accessor<float>()));
//  static_assert(test<t, t, t, t, t, t, t, t, preview::default_accessor<const float>>(preview::default_accessor<float>()));
//  static_assert(test<t, t, t, t, t, t, t, t, preview::default_accessor<MinimalElementType>>(
//      preview::default_accessor<MinimalElementType>()));
//  static_assert(test<t, t, t, t, t, t, t, t, preview::default_accessor<const MinimalElementType>>(
//      preview::default_accessor<MinimalElementType>()));
//  static_assert(test<t, t, t, t, t, t, t, t, checked_accessor<int>>(checked_accessor<int>(1024)));
//  static_assert(test<t, o, t, o, t, t, t, t, checked_accessor<const int>>(checked_accessor<int>(1024)));
//  static_assert(test<t, t, t, t, o, o, o, o, checked_accessor<const unsigned>>(checked_accessor<unsigned>(1024)));
//  static_assert(test<t, t, t, t, t, t, t, t, checked_accessor<float>>(checked_accessor<float>(1024)));
//  static_assert(test<t, t, t, t, t, t, t, t, checked_accessor<double>>(checked_accessor<double>(1024)));
//  static_assert(
//      test<t, t, t, t, t, t, t, t, checked_accessor<MinimalElementType>>(checked_accessor<MinimalElementType>(1024)));
//  static_assert(test<t, o, t, o, t, t, t, t, checked_accessor<const MinimalElementType>>(
//      checked_accessor<MinimalElementType>(1024)));
//  static_assert(
//      test<t, o, t, o, t, t, t, t, conv_test_accessor_c<int, t, t, t, t>>(conv_test_accessor_nc<int, t, t>()));
//  static_assert(
//      test<t, o, t, t, t, t, t, t, conv_test_accessor_c<int, t, t, o, o>>(conv_test_accessor_nc<int, t, o>()));
//  static_assert(
//      test<t, t, t, o, t, t, t, t, conv_test_accessor_c<int, o, t, t, t>>(conv_test_accessor_nc<int, t, t>()));
//  static_assert(
//      test<t, t, t, t, t, t, t, t, conv_test_accessor_c<int, o, o, o, o>>(conv_test_accessor_nc<int, t, o>()));
//  static_assert(test<o, o, o, o, o, o, o, o, preview::default_accessor<float>>(preview::default_accessor<int>()));
//  static_assert(test<o, o, o, o, o, o, o, o, checked_accessor<const double>>(checked_accessor<double>(1024)));
//  static_assert(test<o, o, t, t, t, t, t, t, checked_accessor<const float>>(checked_accessor<float>(1024)));
//  static_assert(
//      test<o, o, o, o, t, t, t, t, conv_test_accessor_c<int, o, o, t, t>>(conv_test_accessor_nc<int, o, o>()));
//  static_assert(
//      test<o, o, t, o, t, t, t, t, conv_test_accessor_c<int, o, t, o, o>>(conv_test_accessor_nc<int, o, t>()));
//  static_assert(
//      test<o, o, t, t, t, t, t, t, conv_test_accessor_c<int, o, o, t, t>>(conv_test_accessor_nc<int, o, t>()));
}

#endif
