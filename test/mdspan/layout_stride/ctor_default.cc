
#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "../../test_utils.h"
#include "../print_to.h"

template <class E>
constexpr void test_construction() {
  using M = preview::layout_stride::mapping<E>;
  ASSERT_NOEXCEPT(M{});
  M m;
  E e;

  // check correct extents are returned
  ASSERT_NOEXCEPT(m.extents());
  EXPECT_EQ(m.extents(), e);

  // check required_span_size()
  typename E::index_type expected_size = 1;
  for (typename E::rank_type r = 0; r < E::rank(); r++)
    expected_size *= e.extent(r);
  EXPECT_EQ(m.required_span_size(), expected_size);

  // check strides: node stride function is constrained on rank>0, e.extent(r) is not
  auto strides = m.strides();
  ASSERT_NOEXCEPT(m.strides());
#if PREVIEW_CXX_VERSION >= 17
  if PREVIEW_CONSTEXPR_AFTER_CXX17 (E::rank() > 0) {
    preview::layout_right::mapping<E> m_right;
    for (typename E::rank_type r = 0; r < E::rank(); r++) {
      ASSERT_EQ(m.stride(r), m_right.stride(r));
      ASSERT_EQ(strides[r], m.stride(r));
    }
  }
#endif
}

TEST(MdSpanLayoutStride, VERSIONED(ctor_default)) {
  constexpr size_t D = preview::dynamic_extent;
  test_construction<preview::extents<int>>();
  test_construction<preview::extents<unsigned, D>>();
  test_construction<preview::extents<unsigned, 7>>();
  test_construction<preview::extents<unsigned, 0>>();
  test_construction<preview::extents<unsigned, 7, 8>>();
  test_construction<preview::extents<int64_t, D, 8, D, D>>();
}
