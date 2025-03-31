
#include "preview/mdspan.h"

#include <array>
#include <cstddef>
#include <ostream>

#include "gtest.h"
#include "../print_to.h"

#include "../CustomTestLayouts.h"

template<typename E, typename = void>
struct layout_mapping_constructible : std::false_type {};
template<typename E>
struct layout_mapping_constructible<E, preview::void_t<
    decltype(preview::layout_stride::mapping<E>(
        std::declval<E&>(),
        std::declval<std::array<typename E::index_type, E::rank()>&>())
    )
>> : std::true_type {};

template <class E1, class E2, bool = layout_mapping_constructible<E1>::value && layout_mapping_constructible<E2>::value>
struct layout_mapping_comparable : std::false_type {};

template <class E1, class E2>
struct layout_mapping_comparable<E1, E2, true>
    : preview::rel_ops::is_equality_comparable<
        preview::layout_stride::mapping<E1>&,
        preview::layout_stride::mapping<E2>&
    > {};

template <class T1, class T2>
constexpr void test_comparison_different_rank() {
  constexpr size_t D = preview::dynamic_extent;

  using E1 = preview::extents<T1, D>;
  using E2 = preview::extents<T2, D>;
  E1 e1;
  E2 e2;
  std::array<typename E1::index_type, E1::rank()> s1{};
  std::array<typename E2::index_type, E2::rank()> s2{};

  preview::layout_stride::mapping<E1> m1(e1, s1);
  preview::layout_stride::mapping<E2> m2(e2, s2);
//  m1 == m2;

  // sanity check same rank
  static_assert(layout_mapping_comparable<preview::extents<T1, D>, preview::extents<T2, D>>::value, "");
  static_assert(layout_mapping_comparable<preview::extents<T1, 5>, preview::extents<T2, D>>::value, "");
  static_assert(layout_mapping_comparable<preview::extents<T1, D>, preview::extents<T2, 5>>::value, "");
  static_assert(layout_mapping_comparable<preview::extents<T1, 5>, preview::extents<T2, 5>>::value, "");

  // not equality comparable when rank is not the same
  static_assert(!layout_mapping_comparable<preview::extents<T1>, preview::extents<T2, D>>::value, "");
  static_assert(!layout_mapping_comparable<preview::extents<T1>, preview::extents<T2, 1>>::value, "");
  static_assert(!layout_mapping_comparable<preview::extents<T1, D>, preview::extents<T2>>::value, "");
  static_assert(!layout_mapping_comparable<preview::extents<T1, 1>, preview::extents<T2>>::value, "");
  static_assert(!layout_mapping_comparable<preview::extents<T1, D>, preview::extents<T2, D, D>>::value, "");
  static_assert(!layout_mapping_comparable<preview::extents<T1, 5>, preview::extents<T2, 5, D>>::value, "");
  static_assert(!layout_mapping_comparable<preview::extents<T1, 5>, preview::extents<T2, 5, 1>>::value, "");
  static_assert(!layout_mapping_comparable<preview::extents<T1, D, D>, preview::extents<T2, D>>::value, "");
  static_assert(!layout_mapping_comparable<preview::extents<T1, 5, D>, preview::extents<T2, 5>>::value, "");
  static_assert(!layout_mapping_comparable<preview::extents<T1, 5, 1>, preview::extents<T2, 5>>::value, "");
}

template <class To, class From>
constexpr void test_comparison(
    bool equal,
    To dest_exts,
    From src_exts,
    std::array<int, To::rank()> dest_strides,
    std::array<int, From::rank()> src_strides) {
  preview::layout_stride::mapping<To> dest(dest_exts, dest_strides);
  preview::layout_stride::mapping<From> src(src_exts, src_strides);
  ASSERT_NOEXCEPT(dest == src);
  ASSERT_EQ((dest == src), equal);
  ASSERT_EQ((dest != src), !equal);
}

template <class T1, class T2>
constexpr void test_comparison_same_rank() {
  constexpr size_t D = preview::dynamic_extent;

  test_comparison(true, preview::extents<T1>(), preview::extents<T2>(), std::array<int, 0>{}, std::array<int, 0>{});

  test_comparison(true, preview::extents<T1, D>(5), preview::extents<T2, D>(5), std::array<int, 1>{1}, std::array<int, 1>{1});
  test_comparison(true, preview::extents<T1, D>(0), preview::extents<T2, D>(0), std::array<int, 1>{1}, std::array<int, 1>{1});
  test_comparison(true, preview::extents<T1, 5>(), preview::extents<T2, D>(5), std::array<int, 1>{3}, std::array<int, 1>{3});
  test_comparison(true, preview::extents<T1, D>(5), preview::extents<T2, 5>(), std::array<int, 1>{1}, std::array<int, 1>{1});
  test_comparison(true, preview::extents<T1, 5>(), preview::extents< T2, 5>(), std::array<int, 1>{1}, std::array<int, 1>{1});
  test_comparison(false, preview::extents<T1, 5>(), preview::extents<T2, D>(5), std::array<int, 1>{2}, std::array<int, 1>{1});
  test_comparison(false, preview::extents<T1, D>(5), preview::extents<T2, D>(5), std::array<int, 1>{2}, std::array<int, 1>{1});
  test_comparison(false, preview::extents<T1, D>(5), preview::extents<T2, D>(7), std::array<int, 1>{1}, std::array<int, 1>{1});
  test_comparison(false, preview::extents<T1, 5>(), preview::extents<T2, D>(7), std::array<int, 1>{1}, std::array<int, 1>{1});
  test_comparison(false, preview::extents<T1, D>(5), preview::extents<T2, 7>(), std::array<int, 1>{1}, std::array<int, 1>{1});
  test_comparison(false, preview::extents<T1, 5>(), preview::extents<T2, 7>(), std::array<int, 1>{1}, std::array<int, 1>{1});

  test_comparison(
      true,
      preview::extents<T1, D, D, D, D, D>(5, 6, 7, 8, 9),
      preview::extents<T2, D, D, D, D, D>(5, 6, 7, 8, 9),
      std::array<int, 5>{2, 20, 200, 2000, 20000},
      std::array<int, 5>{2, 20, 200, 2000, 20000});
  test_comparison(
      true,
      preview::extents<T1, D, 6, D, 8, D>(5, 7, 9),
      preview::extents<T2, 5, D, D, 8, 9>(6, 7),
      std::array<int, 5>{2, 20, 200, 2000, 20000},
      std::array<int, 5>{2, 20, 200, 2000, 20000});
  test_comparison(
      true,
      preview::extents<T1, 5, 6, 7, 8, 9>(5, 6, 7, 8, 9),
      preview::extents<T2, 5, 6, 7, 8, 9>(),
      std::array<int, 5>{2, 20, 200, 2000, 20000},
      std::array<int, 5>{2, 20, 200, 2000, 20000});
  test_comparison(
      false,
      preview::extents<T1, 5, 6, 7, 8, 9>(5, 6, 7, 8, 9),
      preview::extents<T2, 5, 6, 7, 8, 9>(),
      std::array<int, 5>{2, 20, 200, 20000, 2000},
      std::array<int, 5>{2, 20, 200, 2000, 20000});
  test_comparison(
      false,
      preview::extents<T1, D, D, D, D, D>(5, 6, 7, 8, 9),
      preview::extents<T2, D, D, D, D, D>(5, 6, 3, 8, 9),
      std::array<int, 5>{2, 20, 200, 2000, 20000},
      std::array<int, 5>{2, 20, 200, 2000, 20000});
  test_comparison(
      false,
      preview::extents<T1, D, 6, D, 8, D>(5, 7, 9),
      preview::extents<T2, 5, D, D, 3, 9>(6, 7),
      std::array<int, 5>{2, 20, 200, 2000, 20000},
      std::array<int, 5>{2, 20, 200, 2000, 20000});
  test_comparison(
      false,
      preview::extents<T1, 5, 6, 7, 8, 9>(5, 6, 7, 8, 9),
      preview::extents<T2, 5, 6, 7, 3, 9>(),
      std::array<int, 5>{2, 20, 200, 2000, 20000},
      std::array<int, 5>{2, 20, 200, 2000, 20000});
}

template <class OtherLayout, class E1, class E2, class... OtherArgs>
testing::AssertionResult test_comparison_with(
    bool expect_equal, E1 e1, std::array<typename E1::index_type, E1::rank()> strides, E2 e2, OtherArgs... other_args) {
  preview::layout_stride::mapping<E1> map(e1, strides);
  typename OtherLayout::template mapping<E2> other_map(e2, other_args...);

  if (expect_equal) {
    if (map == other_map && other_map == map) {
      return testing::AssertionSuccess();
    }
    return testing::AssertionFailure() << "Expected equal but: " << testing::PrintToString(map) << " != " << testing::PrintToString(other_map);
  } else {
    if (map != other_map && other_map != map) {
      return testing::AssertionSuccess();
    }
    return testing::AssertionFailure() << "Expected not equal but: " << testing::PrintToString(map) << " == " << testing::PrintToString(other_map);
  }
}

template <class OtherLayout>
constexpr void test_comparison_with() {
  constexpr size_t D = preview::dynamic_extent;
  bool is_left_based =
      std::is_same<OtherLayout, preview::layout_left>::value || std::is_same<OtherLayout, always_convertible_layout>::value;
  EXPECT_TRUE(test_comparison_with<OtherLayout>(true, preview::extents<int>(), std::array<int, 0>{}, preview::extents<unsigned>()));
  EXPECT_TRUE(test_comparison_with<OtherLayout>(true, preview::extents<int, 5>(), std::array<int, 1>{1}, preview::extents<unsigned, 5>()));
  EXPECT_TRUE(test_comparison_with<OtherLayout>(true, preview::extents<int, D>(5), std::array<int, 1>{1}, preview::extents<unsigned, 5>()));
  EXPECT_TRUE(test_comparison_with<OtherLayout>(false, preview::extents<int, D>(5), std::array<int, 1>{2}, preview::extents<unsigned, 5>()));
  EXPECT_TRUE(test_comparison_with<OtherLayout>(
      is_left_based, preview::dextents<int, 2>(5, 7), std::array<int, 2>{1, 5}, preview::dextents<unsigned, 2>(5, 7)));
  EXPECT_TRUE(test_comparison_with<OtherLayout>(
      !is_left_based, preview::extents<int, D, D>(5, 7), std::array<int, 2>{7, 1}, preview::extents<unsigned, D, D>(5, 7)));
  EXPECT_TRUE(test_comparison_with<OtherLayout>(
      false, preview::extents<int, D, D>(5, 7), std::array<int, 2>{8, 1}, preview::extents<unsigned, D, D>(5, 7)));

#if PREVIEW_CXX_VERSION >= 17
  if constexpr (std::is_same_v<OtherLayout, always_convertible_layout>) {
    // test layout with strides not equal to product of extents
    test_comparison_with<OtherLayout>(
        true, preview::extents<int, D, D>(5, 7), std::array<int, 2>{2, 10}, preview::extents<unsigned, D, D>(5, 7), 0, 2);
    // make sure that offset != 0 results in false
    test_comparison_with<OtherLayout>(
        false, preview::extents<int, D, D>(5, 7), std::array<int, 2>{2, 10}, preview::extents<unsigned, D, D>(5, 7), 1, 2);
  }
#endif
}

template <class T1, class T2>
constexpr void test_comparison_index_type() {
  test_comparison_same_rank<T1, T2>();
  test_comparison_different_rank<T1, T2>();
  test_comparison_with<preview::layout_right>();
  test_comparison_with<preview::layout_left>();
  test_comparison_with<always_convertible_layout>();
}

TEST(MdSpanLayoutStride, VERSIONED(comparison)) {
  test_comparison_index_type<int, int>();
  test_comparison_index_type<int, size_t>();
  test_comparison_index_type<size_t, int>();
  test_comparison_index_type<size_t, long>();
}
