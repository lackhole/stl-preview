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
constexpr void test_required_span_size(E e, typename E::index_type expected_size) {
  using M = preview::layout_left::mapping<E>;
  const M m(e);

  ASSERT_NOEXCEPT(m.required_span_size());
  assert(m.required_span_size() == expected_size);
}

TEST(MdSpanLayoutLeft, VERSIONED(required_span_size)) {
  constexpr size_t D = preview::dynamic_extent;

  test_required_span_size(preview::extents<int>(), 1);
  test_required_span_size(preview::extents<unsigned, D>(0), 0);
  test_required_span_size(preview::extents<unsigned, D>(1), 1);
  test_required_span_size(preview::extents<unsigned, D>(7), 7);
  test_required_span_size(preview::extents<unsigned, 7>(), 7);
  test_required_span_size(preview::extents<unsigned, 7, 8>(), 56);
  test_required_span_size(preview::extents<int64_t, D, 8, D, D>(7, 9, 10), 5040);
  test_required_span_size(preview::extents<int64_t, 1, 8, D, D>(9, 10), 720);
  test_required_span_size(preview::extents<int64_t, 1, 0, D, D>(9, 10), 0);
}
