//
// Created by yonggyulee on 2024. 11. 6.
//

#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "../../test_utils.h"
#include "../ConvertibleToIntegral.h"
#include "../print_to.h"

template <class E>
constexpr void test_layout_mapping_left() {
  using M = preview::layout_left::mapping<E>;
  ASSERT_TRUE(M::is_unique());
  ASSERT_TRUE(M::is_exhaustive());
  ASSERT_TRUE(M::is_strided());
  ASSERT_TRUE(M::is_always_unique());
  ASSERT_TRUE(M::is_always_exhaustive());
  ASSERT_TRUE(M::is_always_strided());
  ASSERT_NOEXCEPT(std::declval<M>().is_unique());
  ASSERT_NOEXCEPT(std::declval<M>().is_exhaustive());
  ASSERT_NOEXCEPT(std::declval<M>().is_strided());
  ASSERT_NOEXCEPT(M::is_always_unique());
  ASSERT_NOEXCEPT(M::is_always_exhaustive());
  ASSERT_NOEXCEPT(M::is_always_strided());
}

TEST(MdSpanLayoutLeft, VERSIONED(properties)) {
  constexpr size_t D = preview::dynamic_extent;

  test_layout_mapping_left<preview::extents<int>>();
  test_layout_mapping_left<preview::extents<signed char, 4, 5>>();
  test_layout_mapping_left<preview::extents<unsigned, D, 4>>();
  test_layout_mapping_left<preview::extents<size_t, D, D, D, D>>();
}
