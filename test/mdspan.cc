#include "preview/mdspan.h"

#include "gtest.h"

#include "preview/algorithm.h"
#include "preview/array.h"
#include "preview/ranges.h"
#include "preview/span.h"

using namespace std::literals;
namespace ranges = preview::ranges;
namespace views = preview::views;

using preview::dynamic_extent;

TEST(VERSIONED(Mdspan), extents) {
  static_assert(preview::extents<int, 3, 4>::rank() == 2, "");
  static_assert(preview::extents<int, 3, 4>::rank_dynamic() == 0, "");
  static_assert(preview::extents<int, 3, 4, dynamic_extent>::rank_dynamic() == 1, "");
  static_assert(preview::extents<int, dynamic_extent, dynamic_extent>::rank_dynamic() == 2, "");
  static_assert(preview::extents<int, dynamic_extent, 10, dynamic_extent>::rank_dynamic() == 2, "");
  static_assert(preview::extents<int, dynamic_extent, dynamic_extent, 10>::rank_dynamic() == 2, "");
  static_assert(preview::extents<int, dynamic_extent, 10, dynamic_extent, 10>::rank_dynamic() == 2, "");
  static_assert(preview::extents<int, 10, dynamic_extent, 10, dynamic_extent, 10>::rank_dynamic() == 2, "");

  static_assert(sizeof(preview::extents<int, 3, 4>) == 1, "");
  static_assert(sizeof(preview::extents<int, 3, 4, dynamic_extent>) == sizeof(int), "");

  static_assert(preview::extents<int, 3, 4, 5>{}.extent(0) == 3, "");
  static_assert(preview::extents<int, 3, 4, 5>::static_extent(0) == 3, "");
  static_assert(preview::extents<int, 3, 4, 5>{}.extent(1) == 4, "");
  static_assert(preview::extents<int, 3, 4, 5>::static_extent(1) == 4, "");
  static_assert(preview::extents<int, 3, 4, 5>{}.extent(2) == 5, "");
  static_assert(preview::extents<int, 3, 4, 5>::static_extent(2) == 5, "");

  static_assert(preview::extents<int, 3, dynamic_extent, 5>{}.extent(0) == 3, "");
  static_assert(preview::extents<int, 3, dynamic_extent, 5>::static_extent(0) == 3, "");
  static_assert(preview::extents<int, 3, dynamic_extent, 5>{}.extent(1) == 0, "");
  static_assert(preview::extents<int, 3, dynamic_extent, 5>::static_extent(1) == dynamic_extent, "");
  static_assert(preview::extents<int, 3, dynamic_extent, 5>{}.extent(2) == 5, "");
  static_assert(preview::extents<int, 3, dynamic_extent, 5>::static_extent(2) == 5, "");

  constexpr preview::extents<int, 3, 4, 5> e1;
  constexpr preview::extents<int, 3, 4, 5> e2 = e1;
  static_assert(e1 == e2, "");

  constexpr preview::extents<int, 3, dynamic_extent, 5> e3{3, 10, 5};
  constexpr preview::extents<int, 3, dynamic_extent, 5> e4{10};
  static_assert(e3 == e4, "");
  static_assert(e1 != e3, "");

  constexpr preview::extents<int, dynamic_extent, dynamic_extent, dynamic_extent> e5 = e3;
  static_assert(e5 == e3, "");
  static_assert(e5 != e2, "");

  static_assert(preview::extents<int>{} == preview::extents<unsigned int>{}, "");
  static_assert(preview::extents<int, dynamic_extent>{} != preview::extents<unsigned int>{}, "");
  static_assert(preview::extents<int, dynamic_extent>{} != preview::extents<unsigned int, 1, 2>{}, "");

  constexpr preview::dims<5> d1{1, 2, 3, 4, 5};
  preview::span<const int, 5> s{1, 2, 3, 4, 5};
  preview::dextents<int, 5> d2 = s;
  EXPECT_EQ(d1, d2);

  preview::dextents<int, 5> d3 = preview::to_array({1, 2, 3, 4, 5});
  EXPECT_EQ(d1, d3);

  constexpr preview::extents<int, 1, 2, 3, 4, 5> e6;
  EXPECT_EQ(d1, e6);

  preview::extents<int, 1, 2, dynamic_extent, 4, dynamic_extent> e7(d3);
  EXPECT_EQ(d1, e7);

  namespace detail = preview::detail;
  using seq = std::index_sequence<2, 3, 4, 5>;

  static_assert(e7.rank_dynamic() == 2, "");

  {
    constexpr preview::extents<int, 1, 2, dynamic_extent, 4, dynamic_extent> e8(3, 5);
    static_assert(detail::extents_helper::extent_dynamic(e8, 0) == 3, "");
    static_assert(detail::extents_helper::extent_dynamic(e8, 1) == 5, "");
  }
}

TEST(VERSIONED(MdSpan), layout_stride_required_span_size) {
  auto mapping_span_size = [](auto e, auto strides) {
    return preview::layout_stride::mapping<decltype(e)>{e, strides}.required_span_size();
  };

  constexpr auto dynamic = preview::dynamic_extent;

  EXPECT_EQ(mapping_span_size(preview::extents<int>(), std::array<int, 0>{}), 1);
  EXPECT_EQ(mapping_span_size(preview::extents<unsigned, dynamic>(0), std::array<int, 1>{5}), 0);
  EXPECT_EQ(mapping_span_size(preview::extents<unsigned, dynamic>(1), std::array<int, 1>{5}), 1);

  EXPECT_EQ(mapping_span_size(preview::extents<unsigned, dynamic>(7), std::array<int, 1>{5}), 31);
  EXPECT_EQ(mapping_span_size(preview::extents<unsigned, 7>(), std::array<int, 1>{5}), 31);
  EXPECT_EQ(mapping_span_size(preview::extents<unsigned, 7, 8>(), std::array<int, 2>{20, 2}), 135);
  EXPECT_EQ(mapping_span_size(
      preview::extents<int64_t, dynamic, 8, dynamic, dynamic>(7, 9, 10),
      std::array<int, 4>{1, 7, 7 * 8, 7 * 8 * 9}), 5040);
  EXPECT_EQ(mapping_span_size(
      preview::extents<int64_t, 1, 8, dynamic, dynamic>(9, 10),
      std::array<int, 4>{1, 7, 7 * 8, 7 * 8 * 9}), 5034);
  EXPECT_EQ(mapping_span_size(
      preview::extents<int64_t, 1, 0, dynamic, dynamic>(9, 10),
      std::array<int, 4>{1, 7, 7 * 8, 7 * 8 * 9}), 0);

  preview::layout_stride::mapping<preview::extents<int, 2, 3>> m{preview::extents<int, 2, 3>{}, std::array<int, 2>{3, 1}};
  m.is_exhaustive();
}
