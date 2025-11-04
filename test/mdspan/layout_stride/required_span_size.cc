
#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "../../test_utils.h"
#include "../print_to.h"

template <class E>
constexpr void test_required_span_size(E e, std::array<int, E::rank()> strides, typename E::index_type expected_size) {
  using M = preview::layout_stride::mapping<E>;
  const M m(e, strides);

  ASSERT_NOEXCEPT(m.required_span_size());
  EXPECT_EQ(m.required_span_size(), expected_size);
}

TEST(MdSpanLayoutStride, VERSIONED(required_span_size)) {
  constexpr size_t D = preview::dynamic_extent;
  test_required_span_size(preview::extents<int>(), std::array<int, 0>{}, 1);
  test_required_span_size(preview::extents<unsigned, D>(0), std::array<int, 1>{5}, 0);
  test_required_span_size(preview::extents<unsigned, D>(1), std::array<int, 1>{5}, 1);
  test_required_span_size(preview::extents<unsigned, D>(7), std::array<int, 1>{5}, 31);
  test_required_span_size(preview::extents<unsigned, 7>(), std::array<int, 1>{5}, 31);
  test_required_span_size(preview::extents<unsigned, 7, 8>(), std::array<int, 2>{20, 2}, 135);
  test_required_span_size(
      preview::extents<int64_t, D, 8, D, D>(7, 9, 10), std::array<int, 4>{1, 7, 7 * 8, 7 * 8 * 9}, 5040);
  test_required_span_size(preview::extents<int64_t, 1, 8, D, D>(9, 10), std::array<int, 4>{1, 7, 7 * 8, 7 * 8 * 9}, 5034);
  test_required_span_size(preview::extents<int64_t, 1, 0, D, D>(9, 10), std::array<int, 4>{1, 7, 7 * 8, 7 * 8 * 9}, 0);
}
