#include "preview/mdspan.h"

#include "gtest.h"

#include "preview/ranges.h"

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
  static_assert(preview::extents<int, 3, 4, 5>{}.extent(1) == 4, "");
  static_assert(preview::extents<int, 3, 4, 5>{}.extent(2) == 5, "");

  static_assert(preview::extents<int, 3, dynamic_extent, 5>{}.extent(0) == 3, "");
  static_assert(preview::extents<int, 3, dynamic_extent, 5>{}.extent(1) == 0, "");
  static_assert(preview::extents<int, 3, dynamic_extent, 5>{}.extent(2) == 5, "");

  preview::extents<int, 3, 4, 5> e1;
  preview::extents<int, 3, 4, 5> e2 = e1;
  (void)e2;
}
