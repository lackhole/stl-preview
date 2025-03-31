//
// Created by yonggyulee on 2024. 11. 6.
//

#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "gtest.h"
#include "../print_to.h"


template <class To, class From>
constexpr void test_implicit_conversion(To dest, From src) {
  ASSERT_EQ(dest.extents(), src.extents());
}

template <bool implicit, class ToE, class FromE, std::enable_if_t<implicit == false, int> = 0>
constexpr void test_conversion(FromE src_exts) {
  using To   = preview::layout_left::mapping<ToE>;
  using From = preview::layout_right::mapping<FromE>;
  From src(src_exts);

  ASSERT_NOEXCEPT(To(src));
  To dest(src);

  ASSERT_EQ(dest.extents(), src.extents());
}

template <bool implicit, class ToE, class FromE, std::enable_if_t<implicit, int> = 0>
constexpr void test_conversion(FromE src_exts) {
  using To   = preview::layout_left::mapping<ToE>;
  using From = preview::layout_right::mapping<FromE>;
  From src(src_exts);

  ASSERT_NOEXCEPT(To(src));
  To dest(src);

  ASSERT_EQ(dest.extents(), src.extents());
  if PREVIEW_CONSTEXPR_AFTER_CXX17 (implicit) {
    dest = src;
    ASSERT_EQ(dest.extents(), src.extents());
    test_implicit_conversion<To, From>(src, src);
  }
}

template <class T1, class T2>
constexpr void test_conversion() {
  constexpr size_t D = preview::dynamic_extent;
  constexpr bool idx_convertible =
      static_cast<size_t>(std::numeric_limits<T1>::max()) >= static_cast<size_t>(std::numeric_limits<T2>::max());

  // clang-format off
  test_conversion<idx_convertible && true,  preview::extents<T1>>(preview::extents<T2>());
  test_conversion<idx_convertible && true,  preview::extents<T1, D>>(preview::extents<T2, D>(5));
  test_conversion<idx_convertible && false, preview::extents<T1, 5>>(preview::extents<T2, D>(5));
  test_conversion<idx_convertible && true,  preview::extents<T1, 5>>(preview::extents<T2, 5>());
  // clang-format on
}

template <class IdxT, size_t... Extents>
using lr_mapping_t = preview::layout_right::mapping<preview::extents<IdxT, Extents...>>;
template <class IdxT, size_t... Extents>
using ll_mapping_t = preview::layout_left::mapping<preview::extents<IdxT, Extents...>>;

constexpr void test_no_implicit_conversion() {
  constexpr size_t D = preview::dynamic_extent;

  // Sanity check that one static to dynamic conversion works
  static_assert(std::is_constructible<ll_mapping_t<int, D>, lr_mapping_t<int, 5>>::value, "");
  static_assert(std::is_convertible<lr_mapping_t<int, 5>, ll_mapping_t<int, D>>::value, "");

  // Check that dynamic to static conversion only works explicitly
  static_assert(std::is_constructible<ll_mapping_t<int, 5>, lr_mapping_t<int, D>>::value, "");
  static_assert(!std::is_convertible<lr_mapping_t<int, D>, ll_mapping_t<int, 5>>::value, "");

  // Sanity check that smaller index_type to larger index_type conversion works
  static_assert(std::is_constructible<ll_mapping_t<size_t, 5>, lr_mapping_t<int, 5>>::value, "");
  static_assert(std::is_convertible<lr_mapping_t<int, 5>, ll_mapping_t<size_t, 5>>::value, "");

  // Check that larger index_type to smaller index_type conversion works explicitly only
  static_assert(std::is_constructible<ll_mapping_t<int, 5>, lr_mapping_t<size_t, 5>>::value, "");
  static_assert(!std::is_convertible<lr_mapping_t<size_t, 5>, ll_mapping_t<int, 5>>::value, "");
}

constexpr void test_rank_mismatch() {
  constexpr size_t D = preview::dynamic_extent;

  static_assert(!std::is_constructible<lr_mapping_t<int, D>, lr_mapping_t<int>>::value, "");
  static_assert(!std::is_constructible<lr_mapping_t<int>, lr_mapping_t<int, D, D>>::value, "");
  static_assert(!std::is_constructible<lr_mapping_t<int, D>, lr_mapping_t<int, D, D>>::value, "");
  static_assert(!std::is_constructible<lr_mapping_t<int, D, D, D>, lr_mapping_t<int, D, D>>::value, "");
}

constexpr void test_static_extent_mismatch() {
  static_assert(!std::is_constructible<ll_mapping_t<int, 5>, lr_mapping_t<int, 4>>::value, "");
}

constexpr void test_rank_greater_one() {
  constexpr size_t D = preview::dynamic_extent;

  static_assert(!std::is_constructible<ll_mapping_t<int, D, D>, lr_mapping_t<int, D, D>>::value, "");
  static_assert(!std::is_constructible<ll_mapping_t<int, 1, 1>, lr_mapping_t<int, 1, 1>>::value, "");
  static_assert(!std::is_constructible<ll_mapping_t<int, D, D, D>, lr_mapping_t<int, D, D, D>>::value, "");
}

TEST(MdSpanLayoutLeft, VERSIONED(ctor_layout_right)) {
  test_conversion<int, int>();
  test_conversion<int, size_t>();
  test_conversion<size_t, int>();
  test_conversion<size_t, long>();
  test_no_implicit_conversion();
  test_rank_mismatch();
  test_static_extent_mismatch();
  test_rank_greater_one();
}
