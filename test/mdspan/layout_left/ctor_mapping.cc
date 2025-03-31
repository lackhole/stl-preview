//
// Created by yonggyulee on 2024. 11. 6.
//

#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "gtest.h"
#include "../print_to.h"

template <class To, class From>
constexpr void test_implicit_conversion(To dest, From src, std::true_type) {
  dest = src;
  ASSERT_EQ(dest, src);
  ASSERT_EQ(src, src);
}
template <class To, class From>
constexpr void test_implicit_conversion(To, From, std::false_type) {}

template <bool implicit, class ToE, class FromE>
constexpr void test_conversion(FromE src_exts) {
  using To   = preview::layout_left::mapping<ToE>;
  using From = preview::layout_left::mapping<FromE>;
  From src(src_exts);

  ASSERT_NOEXCEPT(To(src));
  To dest(src);

  ASSERT_EQ(dest, src);
  test_implicit_conversion<To, From>(dest, src, preview::bool_constant<implicit>{});
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
  test_conversion<idx_convertible && false, preview::extents<T1, 5, D>>(preview::extents<T2, D, D>(5, 5));
  test_conversion<idx_convertible && true,  preview::extents<T1, D, D>>(preview::extents<T2, D, D>(5, 5));
  test_conversion<idx_convertible && true,  preview::extents<T1, D, D>>(preview::extents<T2, D, 7>(5));
  test_conversion<idx_convertible && true,  preview::extents<T1, 5, 7>>(preview::extents<T2, 5, 7>());
  test_conversion<idx_convertible && false, preview::extents<T1, 5, D, 8, D, D>>(preview::extents<T2, D, D, 8, 9, 1>(5, 7));
  test_conversion<idx_convertible && true,  preview::extents<T1, D, D, D, D, D>>(
      preview::extents<T2, D, D, D, D, D>(5, 7, 8, 9, 1));
  test_conversion<idx_convertible && true,  preview::extents<T1, D, D, 8, 9, D>>(preview::extents<T2, D, 7, 8, 9, 1>(5));
  test_conversion<idx_convertible && true,  preview::extents<T1, 5, 7, 8, 9, 1>>(preview::extents<T2, 5, 7, 8, 9, 1>());
  // clang-format on
}

template <class IdxT, size_t... Extents>
using mapping_t = preview::layout_left::mapping<preview::extents<IdxT, Extents...>>;

constexpr void test_no_implicit_conversion() {
  constexpr size_t D = preview::dynamic_extent;

  // Sanity check that one static to dynamic conversion works
  static_assert(std::is_constructible<mapping_t<int, D>, mapping_t<int, 5>>::value, "");
  static_assert(std::is_convertible<mapping_t<int, 5>, mapping_t<int, D>>::value, "");

  // Check that dynamic to static conversion only works explicitly
  static_assert(std::is_constructible<mapping_t<int, 5>, mapping_t<int, D>>::value, "");
  static_assert(!std::is_convertible<mapping_t<int, D>, mapping_t<int, 5>>::value, "");

  // Sanity check that one static to dynamic conversion works
  static_assert(std::is_constructible<mapping_t<int, D, 7>, mapping_t<int, 5, 7>>::value, "");
  static_assert(std::is_convertible<mapping_t<int, 5, 7>, mapping_t<int, D, 7>>::value, "");

  // Check that dynamic to static conversion only works explicitly
  static_assert(std::is_constructible<mapping_t<int, 5, 7>, mapping_t<int, D, 7>>::value, "");
  static_assert(!std::is_convertible<mapping_t<int, D, 7>, mapping_t<int, 5, 7>>::value, "");

  // Sanity check that smaller index_type to larger index_type conversion works
  static_assert(std::is_constructible<mapping_t<size_t, 5>, mapping_t<int, 5>>::value, "");
  static_assert(std::is_convertible<mapping_t<int, 5>, mapping_t<size_t, 5>>::value, "");

  // Check that larger index_type to smaller index_type conversion works explicitly only
  static_assert(std::is_constructible<mapping_t<int, 5>, mapping_t<size_t, 5>>::value, "");
  static_assert(!std::is_convertible<mapping_t<size_t, 5>, mapping_t<int, 5>>::value, "");
}

constexpr void test_rank_mismatch() {
  constexpr size_t D = preview::dynamic_extent;

  static_assert(!std::is_constructible<mapping_t<int, D>, mapping_t<int>>::value, "");
  static_assert(!std::is_constructible<mapping_t<int>, mapping_t<int, D, D>>::value, "");
  static_assert(!std::is_constructible<mapping_t<int, D>, mapping_t<int, D, D>>::value, "");
  static_assert(!std::is_constructible<mapping_t<int, D, D, D>, mapping_t<int, D, D>>::value, "");
}

constexpr void test_static_extent_mismatch() {
  constexpr size_t D = preview::dynamic_extent;

  static_assert(!std::is_constructible<mapping_t<int, D, 5>, mapping_t<int, D, 4>>::value, "");
  static_assert(!std::is_constructible<mapping_t<int, 5>, mapping_t<int, 4>>::value, "");
  static_assert(!std::is_constructible<mapping_t<int, 5, D>, mapping_t<int, 4, D>>::value, "");
}

TEST(MdSpanLayoutLeft, VERSIONED(ctor_mapping)) {
  test_conversion<int, int>();
  test_conversion<int, size_t>();
  test_conversion<size_t, int>();
  test_conversion<size_t, long>();
  test_no_implicit_conversion();
  test_rank_mismatch();
  test_static_extent_mismatch();
}
