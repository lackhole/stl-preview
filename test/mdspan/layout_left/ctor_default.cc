//
// Created by yonggyulee on 2024. 11. 6.
//

#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "../../test_utils.h"
#include "../print_to.h"

template <class E>
constexpr void test_construction() {
  using M = preview::layout_left::mapping<E>;
  ASSERT_NOEXCEPT(M{});
  M m;
  E e;

  // check correct extents are returned
  ASSERT_NOEXCEPT(m.extents());
  EXPECT_EQ(m.extents(), e);
  EXPECT_EQ(e, m.extents());

  // check required_span_size()
  typename E::index_type expected_size = 1;
  for (typename E::rank_type r = 0; r < E::rank(); r++)
    expected_size *= e.extent(r);
  EXPECT_EQ(m.required_span_size(), expected_size);
}

TEST(MdSpanLayoutLeft, VERSIONED(ctor_default)) {
  constexpr std::size_t D = preview::dynamic_extent;
  test_construction<preview::extents<int>>();
  test_construction<preview::extents<unsigned, D>>();
  test_construction<preview::extents<unsigned, 7>>();
  test_construction<preview::extents<unsigned, 7, 8>>();
  test_construction<preview::extents<int64_t, D, 8, D, D>>();
}
