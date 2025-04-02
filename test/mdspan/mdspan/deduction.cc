//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// <mdspan>

//  template<class CArray>
//    requires(is_array_v<CArray> && rank_v<CArray> == 1)
//    mdspan(CArray&)
//      -> mdspan<remove_all_extents_t<CArray>, extents<size_t, extent_v<CArray, 0>>>;
//
//  template<class Pointer>
//    requires(is_pointer_v<remove_reference_t<Pointer>>)
//    mdspan(Pointer&&)
//      -> mdspan<remove_pointer_t<remove_reference_t<Pointer>>, extents<size_t>>;
//
//  template<class ElementType, class... Integrals>
//    requires((is_convertible_v<Integrals, size_t> && ...) && sizeof...(Integrals) > 0)
//    explicit mdspan(ElementType*, Integrals...)
//      -> mdspan<ElementType, dextents<size_t, sizeof...(Integrals)>>;            // until C++26
//  template<class ElementType, class... Integrals>
//    requires((is_convertible_v<Integrals, size_t> && ...) && sizeof...(Integrals) > 0)
//    explicit mdspan(ElementType*, Integrals...)
//      -> mdspan<ElementType, extents<size_t, maybe-static-ext<Integrals>...>>;  // since C++26
//
//  template<class ElementType, class OtherIndexType, size_t N>
//    mdspan(ElementType*, span<OtherIndexType, N>)
//      -> mdspan<ElementType, dextents<size_t, N>>;
//
//  template<class ElementType, class OtherIndexType, size_t N>
//    mdspan(ElementType*, const array<OtherIndexType, N>&)
//      -> mdspan<ElementType, dextents<size_t, N>>;
//
//  template<class ElementType, class IndexType, size_t... ExtentsPack>
//    mdspan(ElementType*, const extents<IndexType, ExtentsPack...>&)
//      -> mdspan<ElementType, extents<IndexType, ExtentsPack...>>;
//
//  template<class ElementType, class MappingType>
//    mdspan(ElementType*, const MappingType&)
//      -> mdspan<ElementType, typename MappingType::extents_type,
//                typename MappingType::layout_type>;
//
//  template<class MappingType, class AccessorType>
//    mdspan(const typename AccessorType::data_handle_type&, const MappingType&,
//           const AccessorType&)
//      -> mdspan<typename AccessorType::element_type, typename MappingType::extents_type,
//                typename MappingType::layout_type, AccessorType>;

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

#if PREVIEW_CXX_VERSION >= 17

template <class H, class M, class A>
constexpr void test_mdspan_types(const H& handle, const M& map, const A& acc) {
  using MDS = preview::mdspan<typename A::element_type, typename M::extents_type, typename M::layout_type, A>;

  // deduction from data_handle_type (including non-pointer), mapping and accessor
  EXPECT_EQ_TYPE(decltype(preview::mdspan(handle, map, acc)), MDS);

  if constexpr (std::is_same_v<A, preview::default_accessor<typename A::element_type>>) {
    // deduction from pointer and mapping
    // non-pointer data-handle-types have other accessor
    EXPECT_EQ_TYPE(decltype(preview::mdspan(handle, map)), MDS);
    if constexpr (std::is_same_v<typename M::layout_type, preview::layout_right>) {
      // deduction from pointer and extents
      EXPECT_EQ_TYPE(decltype(preview::mdspan(handle, map.extents())), MDS);
    }
  }
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

struct SizeTIntType {
  size_t val;
  constexpr SizeTIntType(size_t val_) : val(val_) {}
  constexpr operator size_t() const noexcept { return size_t(val); }
};

template <class H, class A, std::enable_if_t<preview::is_deductible_v<preview::mdspan, H, int>, int> = 0>
constexpr bool test_no_layout_deduction_guides(const H& handle, const A&) {
  using T = typename A::element_type;

  // deduction from pointer alone
  EXPECT_EQ_TYPE(decltype(preview::mdspan(handle)), preview::mdspan<T, preview::extents<size_t>>);
  // deduction from pointer and integral like
  EXPECT_EQ_TYPE(decltype(preview::mdspan(handle, 5, SizeTIntType(6))), preview::mdspan<T, preview::dextents<size_t, 2>>);

  // P3029R1: deduction from `integral_constant`
  EXPECT_EQ_TYPE(
      decltype(preview::mdspan(handle, std::integral_constant<size_t, 5>{})), preview::mdspan<T, preview::extents<size_t, 5>>);
  EXPECT_EQ_TYPE(decltype(preview::mdspan(handle, std::integral_constant<size_t, 5>{}, preview::dynamic_extent)),
                   preview::mdspan<T, preview::extents<size_t, 5, preview::dynamic_extent>>);
  EXPECT_EQ_TYPE(
      decltype(preview::mdspan(
          handle, std::integral_constant<size_t, 5>{}, preview::dynamic_extent, std::integral_constant<size_t, 7>{})),
      preview::mdspan<T, preview::extents<size_t, 5, preview::dynamic_extent, 7>>);

  std::array<char, 3> exts{};
  // deduction from pointer and array
  EXPECT_EQ_TYPE(decltype(preview::mdspan(handle, exts)), preview::mdspan<T, preview::dextents<size_t, 3>>);
  // deduction from pointer and span
  EXPECT_EQ_TYPE(decltype(preview::mdspan(handle, preview::span(exts))), preview::mdspan<T, preview::dextents<size_t, 3>>);
  return true;
}

template <class H, class A, std::enable_if_t<!preview::is_deductible_v<preview::mdspan, H, int>, int> = 0>
constexpr bool test_no_layout_deduction_guides(const H&, const A&) {
  return false;
}

template <class H, class A>
constexpr void mixin_layout(const H& handle, const A& acc) {
  mixin_extents(handle, preview::layout_left(), acc);
  mixin_extents(handle, preview::layout_right(), acc);
  mixin_extents(handle, layout_wrapping_integral<4>(), acc);

  // checking that there is no deduction happen for non-pointer handle type
  EXPECT_EQ((test_no_layout_deduction_guides(handle, acc)), (std::is_same<H, typename A::element_type*>::value));
}

template <class T>
constexpr void mixin_accessor() {
  ElementPool<T, 1024> elements;
  mixin_layout(elements.get_ptr(), preview::default_accessor<T>());

  // Using weird accessor/data_handle
  // Make sure they actually got the properties we want to test
  // checked_accessor is noexcept copy constructible except for const double
  checked_accessor<T> acc(1024);
  PREVIEW_STATIC_ASSERT(noexcept(checked_accessor<T>(acc)) != std::is_same<T, const double>::value);
  mixin_layout(typename checked_accessor<T>::data_handle_type(elements.get_ptr()), acc);
}

bool test() {
  mixin_accessor<int>();
  mixin_accessor<const int>();
  mixin_accessor<double>();
  mixin_accessor<const double>();
  mixin_accessor<MinimalElementType>();
  mixin_accessor<const MinimalElementType>();

  // deduction from array alone
  float a[12]{};
  EXPECT_EQ_TYPE(decltype(preview::mdspan(a)), preview::mdspan<float, preview::extents<size_t, 12>>);

  return true;
}

#else
bool test() {
  return true;
}
#endif

int main(int, char**) {
  test();
  return 0;
}
