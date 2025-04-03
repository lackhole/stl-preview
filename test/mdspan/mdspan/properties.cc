//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// <mdspan>

// template<class ElementType, class Extents, class LayoutPolicy = layout_right,
//            class AccessorPolicy = default_accessor<ElementType>>
//   class mdspan {
//   public:
//     static constexpr rank_type rank() noexcept { return extents_type::rank(); }
//     static constexpr rank_type rank_dynamic() noexcept { return extents_type::rank_dynamic(); }
//     static constexpr size_t static_extent(rank_type r) noexcept
//       { return extents_type::static_extent(r); }
//     constexpr index_type extent(rank_type r) const noexcept { return extents().extent(r); }
//
//     constexpr size_type size() const noexcept;
//     [[nodiscard]] constexpr bool empty() const noexcept;
//
//
//     constexpr const extents_type& extents() const noexcept { return map_.extents(); }
//     constexpr const data_handle_type& data_handle() const noexcept { return ptr_; }
//     constexpr const mapping_type& mapping() const noexcept { return map_; }
//     constexpr const accessor_type& accessor() const noexcept { return acc_; }
//     /* per LWG-4021 "mdspan::is_always_meow() should be noexcept" */
//     static constexpr bool is_always_unique() noexcept
//       { return mapping_type::is_always_unique(); }
//     static constexpr bool is_always_exhaustive() noexcept
//       { return mapping_type::is_always_exhaustive(); }
//     static constexpr bool is_always_strided() noexcept
//       { return mapping_type::is_always_strided(); }
//
//     constexpr bool is_unique() const
//       { return map_.is_unique(); }
//     constexpr bool is_exhaustive() const
//       { return map_.is_exhaustive(); }
//     constexpr bool is_strided() const
//       { return map_.is_strided(); }
//     constexpr index_type stride(rank_type r) const
//       { return map_.stride(r); }
//   };
//
// Each specialization MDS of mdspan models copyable and
//    - is_nothrow_move_constructible_v<MDS> is true,
//    - is_nothrow_move_assignable_v<MDS> is true, and
//    - is_nothrow_swappable_v<MDS> is true.
// A specialization of mdspan is a trivially copyable type if its accessor_type, mapping_type, and data_handle_type are trivially copyable types.

#include <type_traits>

#include "preview/concepts.h"
#include "preview/core.h"
#include "preview/mdspan.h"
#include "preview/span.h"

#include "../../test_utils.h"

#include "../MinimalElementType.h"
#include "../CustomTestLayouts.h"

template <class H, class M, class A>
constexpr void test_mdspan_types(const H& handle, const M& map, const A& acc) {
  using MDS = preview::mdspan<typename A::element_type, typename M::extents_type, typename M::layout_type, A>;
  MDS m(handle, map, acc);

  // =====================================
  // Traits for every mdspan
  // =====================================
  PREVIEW_STATIC_ASSERT(preview::copyable<MDS>::value);
  PREVIEW_STATIC_ASSERT(std::is_nothrow_move_constructible<MDS>::value);
  PREVIEW_STATIC_ASSERT(std::is_nothrow_move_assignable<MDS>::value);
  PREVIEW_STATIC_ASSERT(preview::is_nothrow_swappable_v<MDS>);

  // =====================================
  // Invariants coming from data handle
  // =====================================
  // data_handle()
  EXPECT_EQ_TYPE(decltype(m.data_handle()), const H&);
  ASSERT_NOEXCEPT(m.data_handle());
#if PREVIEW_CXX_VERSION >= 17
  if constexpr (preview::equality_comparable<H>::value) {
    EXPECT_EQ(m.data_handle(), handle);
  }
#endif

  // =====================================
  // Invariants coming from extents
  // =====================================

  // extents()
  EXPECT_EQ_TYPE(decltype(m.extents()), const typename MDS::extents_type&);
  ASSERT_NOEXCEPT(m.extents());
  EXPECT_EQ(m.extents(), map.extents());

  // rank()
  EXPECT_EQ_TYPE(decltype(m.rank()), typename MDS::rank_type);
  ASSERT_NOEXCEPT(m.rank());
  EXPECT_EQ(MDS::rank(), MDS::extents_type::rank());

  // rank_dynamic()
  EXPECT_EQ_TYPE(decltype(m.rank_dynamic()), typename MDS::rank_type);
  ASSERT_NOEXCEPT(m.rank_dynamic());
  EXPECT_EQ(MDS::rank_dynamic(), MDS::extents_type::rank_dynamic());

  // extent(r), static_extent(r), size()
  if PREVIEW_CONSTEXPR_AFTER_CXX17 (MDS::rank() > 0) {
    typename MDS::size_type size = 1;
    for (typename MDS::rank_type r = 0; r < MDS::rank(); r++) {
      EXPECT_EQ_TYPE(decltype(MDS::static_extent(r)), size_t);
      ASSERT_NOEXCEPT(MDS::static_extent(r));
      EXPECT_EQ(MDS::static_extent(r), MDS::extents_type::static_extent(r));
      EXPECT_EQ_TYPE(decltype(m.extent(r)), typename MDS::index_type);
      ASSERT_NOEXCEPT(m.extent(r));
      EXPECT_EQ(m.extent(r), m.extents().extent(r));
      size *= m.extent(r);
    }
    EXPECT_EQ(m.size(), size);
  } else {
    EXPECT_EQ(m.size(), 1);
  }
  EXPECT_EQ_TYPE(decltype(m.size()), typename MDS::size_type);
  ASSERT_NOEXCEPT(m.size());

  // empty()
  EXPECT_EQ_TYPE(decltype(m.empty()), bool);
  ASSERT_NOEXCEPT(m.empty());
  EXPECT_EQ(m.empty(), (m.size() == 0));

  // =====================================
  // Invariants coming from mapping
  // =====================================

  // mapping()
  EXPECT_EQ_TYPE(decltype(m.mapping()), const M&);
  ASSERT_NOEXCEPT(m.mapping());

  // is_[always_]unique/exhaustive/strided()
  EXPECT_EQ_TYPE(decltype(MDS::is_always_unique()), bool);
  EXPECT_EQ_TYPE(decltype(MDS::is_always_exhaustive()), bool);
  EXPECT_EQ_TYPE(decltype(MDS::is_always_strided()), bool);
  EXPECT_EQ_TYPE(decltype(m.is_unique()), bool);
  EXPECT_EQ_TYPE(decltype(m.is_exhaustive()), bool);
  EXPECT_EQ_TYPE(decltype(m.is_strided()), bool);
  // per LWG-4021 "mdspan::is_always_meow() should be noexcept"
  PREVIEW_STATIC_ASSERT(noexcept(MDS::is_always_unique()));
  PREVIEW_STATIC_ASSERT(noexcept(MDS::is_always_exhaustive()));
  PREVIEW_STATIC_ASSERT(noexcept(MDS::is_always_strided()));
  PREVIEW_STATIC_ASSERT(!noexcept(m.is_unique()));
  PREVIEW_STATIC_ASSERT(!noexcept(m.is_exhaustive()));
  PREVIEW_STATIC_ASSERT(!noexcept(m.is_strided()));
  PREVIEW_STATIC_ASSERT(MDS::is_always_unique() == M::is_always_unique());
  PREVIEW_STATIC_ASSERT(MDS::is_always_exhaustive() == M::is_always_exhaustive());
  PREVIEW_STATIC_ASSERT(MDS::is_always_strided() == M::is_always_strided());
  EXPECT_EQ(m.is_unique(), map.is_unique());
  EXPECT_EQ(m.is_exhaustive(), map.is_exhaustive());
  EXPECT_EQ(m.is_strided(), map.is_strided());

  // stride(r)
#if PREVIEW_CXX_VERSION >= 17
  if constexpr (MDS::rank() > 0) {
    if (m.is_strided()) {
      for (typename MDS::rank_type r = 0; r < MDS::rank(); r++) {
        EXPECT_EQ_TYPE(decltype(m.stride(r)), typename MDS::index_type);
        PREVIEW_STATIC_ASSERT(!noexcept(m.stride(r)));
        EXPECT_EQ(m.stride(r), map.stride(r));
      }
    }
  }
#endif

  // =====================================
  // Invariants coming from accessor
  // =====================================

  // accessor()
  EXPECT_EQ_TYPE(decltype(m.accessor()), const A&);
  ASSERT_NOEXCEPT(m.accessor());
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
}

constexpr bool test() {
  mixin_accessor<int>();
  mixin_accessor<const int>();
  mixin_accessor<double>();
  mixin_accessor<const double>();
  mixin_accessor<MinimalElementType>();
  mixin_accessor<const MinimalElementType>();
  return true;
}
int main(int, char**) {
  test();
  return 0;
}
