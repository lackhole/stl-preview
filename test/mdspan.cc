#include "preview/mdspan.h"

#include "gtest.h"

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
}
