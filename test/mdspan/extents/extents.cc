//
// Created by yonggyulee on 2024. 10. 28.
//

#include "preview/mdspan.h"
#include "gtest.h"

#include <limits>

template<typename TypesPair>
class MdSpanExtents : public testing::Test {};
using MdSpanExtentsTypes = ::testing::Types<
    std::pair<int, int>,
    std::pair<int, std::size_t>,
    std::pair<std::size_t, int>,
    std::pair<std::size_t, long>>;
TYPED_TEST_SUITE(MdSpanExtents, MdSpanExtentsTypes);

VERSIONED_TYPE_TEST(MdSpanExtents, compare_different_rank) {
  using T1 = typename TypeParam::first_type;
  using T2 = typename TypeParam::second_type;
  constexpr auto D = preview::dynamic_extent;

  ASSERT_NOEXCEPT(preview::extents<T1>() == preview::extents<T2, D>(1));
  EXPECT_FALSE  ((preview::extents<T1>() == preview::extents<T2, D>(1)));

  ASSERT_NOEXCEPT(preview::extents<T1>() == preview::extents<T2, 1>());
  EXPECT_FALSE  ((preview::extents<T1>() == preview::extents<T2, 1>()));

  ASSERT_NOEXCEPT(preview::extents<T1, D>(1) == preview::extents<T2>());
  EXPECT_FALSE  ((preview::extents<T1, D>(1) == preview::extents<T2>()));

  ASSERT_NOEXCEPT(preview::extents<T1, 1>() == preview::extents<T2>());
  EXPECT_FALSE  ((preview::extents<T1, 1>() == preview::extents<T2>()));

  ASSERT_NOEXCEPT(preview::extents<T1, D>(5) == preview::extents<T2, D, D>(5, 5));
  EXPECT_FALSE  ((preview::extents<T1, D>(5) == preview::extents<T2, D, D>(5, 5)));

  ASSERT_NOEXCEPT(preview::extents<T1, 5>() == preview::extents<T2, 5, D>(5));
  EXPECT_FALSE  ((preview::extents<T1, 5>() == preview::extents<T2, 5, D>(5)));

  ASSERT_NOEXCEPT(preview::extents<T1, 5>() == preview::extents<T2, 5, 1>());
  EXPECT_FALSE  ((preview::extents<T1, 5>() == preview::extents<T2, 5, 1>()));

  ASSERT_NOEXCEPT(preview::extents<T1, D, D>(5, 5) == preview::extents<T2, D>(5));
  EXPECT_FALSE  ((preview::extents<T1, D, D>(5, 5) == preview::extents<T2, D>(5)));

  ASSERT_NOEXCEPT(preview::extents<T1, 5, D>(5) == preview::extents<T2, D>(5));
  EXPECT_FALSE  ((preview::extents<T1, 5, D>(5) == preview::extents<T2, D>(5)));

  ASSERT_NOEXCEPT(preview::extents<T1, 5, 5>() == preview::extents<T2, 5>());
  EXPECT_FALSE  ((preview::extents<T1, 5, 5>() == preview::extents<T2, 5>()));
}

VERSIONED_TYPE_TEST(MdSpanExtents, compare_same_rank) {
  using T1 = typename TypeParam::first_type;
  using T2 = typename TypeParam::second_type;
  constexpr auto D = preview::dynamic_extent;

  ASSERT_NOEXCEPT(preview::extents<T1>() == preview::extents<T2>());
  EXPECT_EQ      (preview::extents<T1>(), preview::extents<T2>());

  ASSERT_NOEXCEPT(preview::extents<T1, D>(5) == preview::extents<T2, D>(5));
  EXPECT_TRUE   ((preview::extents<T1, D>(5) == preview::extents<T2, D>(5)));

  ASSERT_NOEXCEPT(preview::extents<T1, 5>() == preview::extents<T2, D>(5));
  EXPECT_TRUE   ((preview::extents<T1, 5>() == preview::extents<T2, D>(5)));

  ASSERT_NOEXCEPT(preview::extents<T1, D>(5) == preview::extents<T2, 5>());
  EXPECT_TRUE   ((preview::extents<T1, D>(5) == preview::extents<T2, 5>()));

  ASSERT_NOEXCEPT(preview::extents<T1, 5>() == preview::extents<T2, 5>());
  EXPECT_TRUE   ((preview::extents<T1, 5>() == preview::extents<T2, 5>()));

  ASSERT_NOEXCEPT(preview::extents<T1, D>(5) == preview::extents<T2, D>(7));
  EXPECT_FALSE  ((preview::extents<T1, D>(5) == preview::extents<T2, D>(7)));

  ASSERT_NOEXCEPT(preview::extents<T1, 5>() == preview::extents<T2, D>(7));
  EXPECT_FALSE  ((preview::extents<T1, 5>() == preview::extents<T2, D>(7)));

  ASSERT_NOEXCEPT(preview::extents<T1, D>(5) == preview::extents<T2, 7>());
  EXPECT_FALSE  ((preview::extents<T1, D>(5) == preview::extents<T2, 7>()));

  ASSERT_NOEXCEPT(preview::extents<T1, 5>() == preview::extents<T2, 7>());
  EXPECT_FALSE  ((preview::extents<T1, 5>() == preview::extents<T2, 7>()));

  ASSERT_NOEXCEPT(preview::extents<T1, D, D, D, D, D>(5, 6, 7, 8, 9) == preview::extents<T2, D, D, D, D, D>(5, 6, 7, 8, 9));
  EXPECT_TRUE   ((preview::extents<T1, D, D, D, D, D>(5, 6, 7, 8, 9) == preview::extents<T2, D, D, D, D, D>(5, 6, 7, 8, 9)));

  ASSERT_NOEXCEPT(preview::extents<T1, D, 6, D, 8, D>(5, 7, 9) == preview::extents<T2, 5, D, D, 8, 9>(6, 7));
  EXPECT_TRUE   ((preview::extents<T1, D, 6, D, 8, D>(5, 7, 9) == preview::extents<T2, 5, D, D, 8, 9>(6, 7)));

  ASSERT_NOEXCEPT(preview::extents<T1, 5, 6, 7, 8, 9>(5, 6, 7, 8, 9) == preview::extents<T2, 5, 6, 7, 8, 9>());
  EXPECT_TRUE   ((preview::extents<T1, 5, 6, 7, 8, 9>(5, 6, 7, 8, 9) == preview::extents<T2, 5, 6, 7, 8, 9>()));

  ASSERT_NOEXCEPT(preview::extents<T1, D, D, D, D, D>(5, 6, 7, 8, 9) == preview::extents<T2, D, D, D, D, D>(5, 6, 3, 8, 9));
  EXPECT_FALSE  ((preview::extents<T1, D, D, D, D, D>(5, 6, 7, 8, 9) == preview::extents<T2, D, D, D, D, D>(5, 6, 3, 8, 9)));

  ASSERT_NOEXCEPT(preview::extents<T1, D, 6, D, 8, D>(5, 7, 9) == preview::extents<T2, 5, D, D, 3, 9>(6, 7));
  EXPECT_FALSE  ((preview::extents<T1, D, 6, D, 8, D>(5, 7, 9) == preview::extents<T2, 5, D, D, 3, 9>(6, 7)));

  ASSERT_NOEXCEPT(preview::extents<T1, 5, 6, 7, 8, 9>(5, 6, 7, 8, 9) == preview::extents<T2, 5, 6, 7, 3, 9>());
  EXPECT_FALSE  ((preview::extents<T1, 5, 6, 7, 8, 9>(5, 6, 7, 8, 9) == preview::extents<T2, 5, 6, 7, 3, 9>()));
}

template <class To, class From>
constexpr void test_implicit_conversion(To dest, From src) {
  EXPECT_EQ(dest, src);
}

template <bool implicit, class To, class From, std::enable_if_t<implicit, int> = 0>
constexpr void test_conversion(From src) {
  To dest(src);
  EXPECT_EQ(dest, src);
  dest = src;
  EXPECT_EQ(dest, src);
  test_implicit_conversion<To, From>(src, src);
}

template <bool implicit, class To, class From, std::enable_if_t<!implicit, int> = 0>
constexpr void test_conversion(From src) {
  To dest(src);
  EXPECT_EQ(dest, src);
}

VERSIONED_TYPE_TEST(MdSpanExtents, conversion) {
  using T1 = typename TypeParam::first_type;
  using T2 = typename TypeParam::second_type;
  constexpr auto D = preview::dynamic_extent;
  constexpr bool idx_convertible =
      static_cast<std::size_t>(std::numeric_limits<T1>::max()) >= static_cast<std::size_t>(std::numeric_limits<T2>::max());

  test_conversion<idx_convertible && true, preview::extents<T1>>(preview::extents<T2>());
  test_conversion<idx_convertible && true, preview::extents<T1, D>>(preview::extents<T2, D>(5));
  test_conversion<idx_convertible && false, preview::extents<T1, 5>>(preview::extents<T2, D>(5));
  test_conversion<idx_convertible && true, preview::extents<T1, 5>>(preview::extents<T2, 5>());
  test_conversion<idx_convertible && false, preview::extents<T1, 5, D>>(preview::extents<T2, D, D>(5, 5));
  test_conversion<idx_convertible && true, preview::extents<T1, D, D>>(preview::extents<T2, D, D>(5, 5));
  test_conversion<idx_convertible && true, preview::extents<T1, D, D>>(preview::extents<T2, D, 7>(5));
  test_conversion<idx_convertible && true, preview::extents<T1, 5, 7>>(preview::extents<T2, 5, 7>());
  test_conversion<idx_convertible && false, preview::extents<T1, 5, D, 8, D, D>>(preview::extents<T2, D, D, 8, 9, 1>(5, 7));
  test_conversion<idx_convertible && true, preview::extents<T1, D, D, D, D, D>>(
      preview::extents<T2, D, D, D, D, D>(5, 7, 8, 9, 1));
  test_conversion<idx_convertible && true, preview::extents<T1, D, D, 8, 9, D>>(preview::extents<T2, D, 7, 8, 9, 1>(5));
  test_conversion<idx_convertible && true, preview::extents<T1, 5, 7, 8, 9, 1>>(preview::extents<T2, 5, 7, 8, 9, 1>());
}

TEST(MdSpanExtents, VERSIONED(no_implicit_conversion)) {
  constexpr size_t D = preview::dynamic_extent;
  // Sanity check that one static to dynamic conversion works
  static_assert(std::is_constructible<preview::extents<int, D>, preview::extents<int, 5>>::value, "");
  static_assert(std::is_convertible<preview::extents<int, 5>, preview::extents<int, D>>::value, "");

  // Check that dynamic to static conversion only works explicitly only
  static_assert(std::is_constructible<preview::extents<int, 5>, preview::extents<int, D>>::value, "");
  static_assert(!std::is_convertible<preview::extents<int, D>, preview::extents<int, 5>>::value, "");

  // Sanity check that one static to dynamic conversion works
  static_assert(std::is_constructible<preview::extents<int, D, 7>, preview::extents<int, 5, 7>>::value, "");
  static_assert(std::is_convertible<preview::extents<int, 5, 7>, preview::extents<int, D, 7>>::value, "");

  // Check that dynamic to static conversion only works explicitly only
  static_assert(std::is_constructible<preview::extents<int, 5, 7>, preview::extents<int, D, 7>>::value, "");
  static_assert(!std::is_convertible<preview::extents<int, D, 7>, preview::extents<int, 5, 7>>::value, "");

  // Sanity check that smaller index_type to larger index_type conversion works
  static_assert(std::is_constructible<preview::extents<size_t, 5>, preview::extents<int, 5>>::value, "");
  static_assert(std::is_convertible<preview::extents<int, 5>, preview::extents<size_t, 5>>::value, "");

  // Check that larger index_type to smaller index_type conversion works explicitly only
  static_assert(std::is_constructible<preview::extents<int, 5>, preview::extents<size_t, 5>>::value, "");
  static_assert(!std::is_convertible<preview::extents<size_t, 5>, preview::extents<int, 5>>::value, "");
}

TEST(MdSpanExtents, VERSIONED(rank_mismatch)) {
  constexpr size_t D = preview::dynamic_extent;

  static_assert(!std::is_constructible<preview::extents<int, D>, preview::extents<int>>::value, "");
  static_assert(!std::is_constructible<preview::extents<int>, preview::extents<int, D, D>>::value, "");
  static_assert(!std::is_constructible<preview::extents<int, D>, preview::extents<int, D, D>>::value, "");
  static_assert(!std::is_constructible<preview::extents<int, D, D, D>, preview::extents<int, D, D>>::value, "");
}

TEST(MdSpanExtents, VERSIONED(static_extent_mismatch)) {
  constexpr size_t D = preview::dynamic_extent;

  static_assert(!std::is_constructible<preview::extents<int, D, 5>, preview::extents<int, D, 4>>::value, "");
  static_assert(!std::is_constructible<preview::extents<int, 5>, preview::extents<int, 4>>::value, "");
  static_assert(!std::is_constructible<preview::extents<int, 5, D>, preview::extents<int, 4, D>>::value, "");
}

#if PREVIEW_CXX_VERSION >= 17

struct NoDefaultCtorIndex {
  std::size_t value;
  constexpr NoDefaultCtorIndex() = delete;
  constexpr NoDefaultCtorIndex(std::size_t val) : value(val) {}
  constexpr operator std::size_t() const noexcept { return value; }
};

TEST(MdSpanExtents, VERSIONED(ctad)) {
  constexpr std::size_t D = preview::dynamic_extent;

  EXPECT_STRONG_EQ(preview::extents(), preview::extents<size_t>());
  EXPECT_STRONG_EQ((preview::extents(1)), (preview::extents<std::size_t, D>(1)));

  EXPECT_STRONG_EQ((preview::extents(1, 2u)), (preview::extents<std::size_t, D, D>(1, 2u)));
  EXPECT_STRONG_EQ((preview::extents(1, 2u, 3, 4, 5, 6, 7, 8, 9)),
                   (preview::extents<std::size_t, D, D, D, D, D, D, D, D, D>(1, 2u, 3, 4, 5, 6, 7, 8, 9)));
  EXPECT_STRONG_EQ((preview::extents(NoDefaultCtorIndex{1}, NoDefaultCtorIndex{2})),
                   (preview::extents<std::size_t, D, D>(1, 2)));

  // P3029R1: deduction from `integral_constant`
  EXPECT_STRONG_EQ((preview::extents(std::integral_constant<size_t, 5>{})),
                   (preview::extents<std::size_t, 5>()));
  EXPECT_STRONG_EQ((preview::extents(std::integral_constant<size_t, 5>{}, 6)),
                   (preview::extents<std::size_t, 5, preview::dynamic_extent>(6)));
  EXPECT_STRONG_EQ((preview::extents(std::integral_constant<size_t, 5>{}, 6, std::integral_constant<size_t, 7>{})),
                   (preview::extents<std::size_t, 5, preview::dynamic_extent, 7>(6)));
}

#endif
