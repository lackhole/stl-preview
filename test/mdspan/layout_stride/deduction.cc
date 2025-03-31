
#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "gtest.h"
#include "mdspan/print_to.h"

#if PREVIEW_CXX_VERSION >= 17

TEST(MdSpanLayoutStride, VERSIONED(deduction)) {
  constexpr size_t D = preview::dynamic_extent;

  EXPECT_EQ_TYPE(decltype(preview::layout_stride::mapping(preview::extents<int>(), std::array<unsigned, 0>())),
                   preview::layout_stride::mapping<preview::extents<int>>);
  EXPECT_EQ_TYPE(decltype(preview::layout_stride::mapping(preview::extents<int, 4>(), std::array<char, 1>{1})),
                   preview::layout_stride::mapping<preview::extents<int, 4>>);
  EXPECT_EQ_TYPE(decltype(preview::layout_stride::mapping(preview::extents<int, D>(), std::array<char, 1>{1})),
                   preview::layout_stride::mapping<preview::extents<int, D>>);
  EXPECT_EQ_TYPE(
      decltype(preview::layout_stride::mapping(preview::extents<unsigned, D, 3>(), std::array<std::int64_t, 2>{3, 100})),
      preview::layout_stride::mapping<preview::extents<unsigned, D, 3>>);

  EXPECT_EQ_TYPE(decltype(preview::layout_stride::mapping(preview::extents<int>(), preview::span<unsigned, 0>())),
                   preview::layout_stride::mapping<preview::extents<int>>);
  EXPECT_EQ_TYPE(decltype(preview::layout_stride::mapping(preview::extents<int, 4>(), std::declval<preview::span<char, 1>>())),
                   preview::layout_stride::mapping<preview::extents<int, 4>>);
  EXPECT_EQ_TYPE(decltype(preview::layout_stride::mapping(preview::extents<int, D>(), std::declval<preview::span<char, 1>>())),
                   preview::layout_stride::mapping<preview::extents<int, D>>);
  EXPECT_EQ_TYPE(
      decltype(preview::layout_stride::mapping(preview::extents<unsigned, D, 3>(), std::declval<preview::span<std::int64_t, 2>>())),
      preview::layout_stride::mapping<preview::extents<unsigned, D, 3>>);
}

#endif
