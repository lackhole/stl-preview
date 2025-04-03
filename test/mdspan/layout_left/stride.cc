//
// Created by yonggyulee on 2024. 11. 6.
//

#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "../../test_utils.h"
#include "../ConvertibleToIntegral.h"
#include "../print_to.h"

template <class E, class... Args>
constexpr void test_stride(std::array<typename E::index_type, E::rank()> strides, Args... args) {
  using M = preview::layout_left::mapping<E>;
  M m(E(args...));

  ASSERT_NOEXCEPT(m.stride(0));
  for (size_t r = 0; r < E::rank(); r++) {
    EXPECT_EQ(strides[r], m.stride(r));
    EXPECT_EQ(m.stride(r), strides[r]);
  }
}

TEST(MdSpanLayoutLeft, VERSIONED(stride)) {
  constexpr size_t D = preview::dynamic_extent;

  test_stride<preview::extents<unsigned, D>>(std::array<unsigned, 1>{1}, 7);
  test_stride<preview::extents<unsigned, 7>>(std::array<unsigned, 1>{1});
  test_stride<preview::extents<unsigned, 7, 8>>(std::array<unsigned, 2>{1, 7});
  test_stride<preview::extents<int64_t, D, 8, D, D>>(std::array<int64_t, 4>{1, 7, 56, 504}, 7, 9, 10);
}
