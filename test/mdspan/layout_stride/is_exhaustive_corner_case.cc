#include "preview/mdspan.h"

#include <array>
#include <cstddef>
#include <ostream>

#include "gtest.h"
#include "../print_to.h"

template <class E>
constexpr void
test_layout_mapping_stride(E ext, std::array<typename E::index_type, E::rank()> strides, bool exhaustive) {
  using M = preview::layout_stride::mapping<E>;
  M m(ext, strides);
  ASSERT_EQ(m.is_exhaustive(), exhaustive);
}

TEST(MdSpanLayoutStride, VERSIONED(is_exhaustive_corner_case)) {
  constexpr size_t D = preview::dynamic_extent;
  test_layout_mapping_stride(preview::extents<int, 0>(), std::array<int, 1>{1}, true);
  test_layout_mapping_stride(preview::extents<unsigned, D>(0), std::array<unsigned, 1>{3}, false);
  test_layout_mapping_stride(preview::extents<int, 0, 3>(), std::array<int, 2>{6, 2}, true);
  test_layout_mapping_stride(preview::extents<int, D, D>(3, 0), std::array<int, 2>{6, 2}, false);
  test_layout_mapping_stride(preview::extents<int, D, D>(0, 0), std::array<int, 2>{6, 2}, false);
  test_layout_mapping_stride(
      preview::extents<unsigned, D, D, D, D>(3, 3, 0, 3), std::array<unsigned, 4>{3, 1, 27, 9}, true);
  test_layout_mapping_stride(preview::extents<int, D, D, D, D>(0, 3, 3, 3), std::array<int, 4>{3, 1, 27, 9}, false);
}
