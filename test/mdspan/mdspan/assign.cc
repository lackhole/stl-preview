//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <mdspan>

// constexpr mdspan& operator=(const mdspan& rhs) = default;

#include "preview/mdspan.h"

#include <type_traits>

#include "preview/core.h"
#include "preview/concepts.h"
#include "preview/span.h"

#include "../../test_utils.h"

#include "../MinimalElementType.h"
#include "../CustomTestLayouts.h"
#include "CustomTestAccessors.h"

template <class H, class M, class A>
constexpr void test_mdspan_types(const H& handle, const M& map, const A& acc) {
  using MDS = preview::mdspan<typename A::element_type, typename M::extents_type, typename M::layout_type, A>;

  MDS m_org(handle, map, acc);
  MDS m(handle, map, acc);

  // The defaulted assignment operator seems to be deprecated because:
  //   error: definition of implicit copy assignment operator for 'checked_accessor<const double>' is deprecated
  //   because it has a user-provided copy constructor [-Werror,-Wdeprecated-copy-with-user-provided-copy]
#if PREVIEW_CXX_VERSION >= 17
  if constexpr (!std::is_same<A, checked_accessor<const double>>::value)
    m = m_org;
#endif
  // even though the following checks out:
  PREVIEW_STATIC_ASSERT(preview::copyable<checked_accessor<const double>>::value);
  PREVIEW_STATIC_ASSERT(std::is_assignable<checked_accessor<const double>, checked_accessor<const double>>::value);

#if PREVIEW_CXX_VERSION >= 17
  PREVIEW_STATIC_ASSERT(noexcept(m = m_org));
#endif
  EXPECT_EQ(m.extents(), map.extents());
  EXPECT_EQ(map.extents(), m.extents());

#if PREVIEW_CXX_VERSION >= 17
  if constexpr (preview::equality_comparable<H>::value){
    EXPECT_EQ(m.data_handle(), handle);
    EXPECT_EQ(handle, m.data_handle());
  }
  if constexpr (preview::equality_comparable<M>::value){
    EXPECT_EQ(m.mapping(), map);
    EXPECT_EQ(map, m.mapping());
  }
  if constexpr (preview::equality_comparable<A>::value){
    EXPECT_EQ(m.accessor(), acc);
    EXPECT_EQ(acc, m.accessor());
  }
#endif

  PREVIEW_STATIC_ASSERT(std::is_trivially_assignable<MDS, const MDS&>::value ==
                ((!std::is_class<H>::value ||
                  std::is_trivially_assignable<H, const H&>::value)&&std::is_trivially_assignable<M, const M&>::value &&
                 std::is_trivially_assignable<A, const A&>::value));
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
  // make sure we test a trivially assignable mapping
  PREVIEW_STATIC_ASSERT(std::is_trivially_assignable<preview::layout_left::mapping<preview::extents<int>>,
                                             const preview::layout_left::mapping<preview::extents<int>>&>::value);
  mixin_extents(handle, preview::layout_left(), acc);
  mixin_extents(handle, preview::layout_right(), acc);
  // make sure we test a not trivially assignable mapping
  PREVIEW_STATIC_ASSERT(!std::is_trivially_assignable< layout_wrapping_integral<4>::mapping<preview::extents<int>>,
                                               const layout_wrapping_integral<4>::mapping<preview::extents<int>>&>::value);
  mixin_extents(handle, layout_wrapping_integral<4>(), acc);
}

template <class T>
constexpr void mixin_accessor() {
  ElementPool<T, 1024> elements;
  // make sure we test trivially constructible accessor and data_handle
  PREVIEW_STATIC_ASSERT(std::is_trivially_copyable<preview::default_accessor<T>>::value);
  PREVIEW_STATIC_ASSERT(std::is_trivially_copyable<typename preview::default_accessor<T>::data_handle_type>::value);
  mixin_layout(elements.get_ptr(), preview::default_accessor<T>());

  // Using weird accessor/data_handle
  // Make sure they actually got the properties we want to test
  // checked_accessor is noexcept copy constructible except for const double
  checked_accessor<T> acc(1024);
  PREVIEW_STATIC_ASSERT(noexcept(checked_accessor<T>(acc)) != std::is_same<T, const double>::value);
  mixin_layout(typename checked_accessor<T>::data_handle_type(elements.get_ptr()), acc);
}

TEST(MdSpan, VERSIONED(assign)) {
  mixin_accessor<int>();
  mixin_accessor<const int>();
  mixin_accessor<double>();
  mixin_accessor<const double>();
  mixin_accessor<MinimalElementType>();
  mixin_accessor<const MinimalElementType>();
}
