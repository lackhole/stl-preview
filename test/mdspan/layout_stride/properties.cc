#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "preview/core.h"

#include "../../test_utils.h"
#include "../print_to.h"

template <class E>
constexpr void
test_layout_mapping_stride(E ext, std::array<typename E::index_type, E::rank()> strides, bool exhaustive) {
  using M = preview::layout_stride::mapping<E>;
  M m(ext, strides);
  const M c_m = m;
  EXPECT_EQ(m.strides(), strides);
  EXPECT_EQ(strides, m.strides());
  EXPECT_EQ(c_m.strides(), strides);
  EXPECT_EQ(strides, c_m.strides());
  EXPECT_EQ(m.extents(), ext);
  EXPECT_EQ(ext, m.extents());
  EXPECT_EQ(c_m.extents(), ext);
  EXPECT_EQ(ext, c_m.extents());
  EXPECT_EQ(m.is_exhaustive(), exhaustive);
  EXPECT_EQ(c_m.is_exhaustive(), exhaustive);
  EXPECT_EQ(M::is_strided(), true);
  EXPECT_EQ(M::is_always_unique(), true);
  EXPECT_EQ(M::is_always_exhaustive(), false);
  EXPECT_EQ(M::is_always_strided(), true);

  ASSERT_NOEXCEPT(m.strides());
  ASSERT_NOEXCEPT(c_m.strides());
  ASSERT_NOEXCEPT(m.extents());
  ASSERT_NOEXCEPT(c_m.extents());
  ASSERT_NOEXCEPT(M::is_unique());
  ASSERT_NOEXCEPT(m.is_exhaustive());
  ASSERT_NOEXCEPT(c_m.is_exhaustive());
  ASSERT_NOEXCEPT(M::is_strided());
  ASSERT_NOEXCEPT(M::is_always_unique());
  ASSERT_NOEXCEPT(M::is_always_exhaustive());
  ASSERT_NOEXCEPT(M::is_always_strided());

  for (typename E::rank_type r = 0; r < E::rank(); r++) {
    ASSERT_EQ(m.stride(r), strides[r]);
    ASSERT_EQ(c_m.stride(r), strides[r]);
    ASSERT_NOEXCEPT(m.stride(r));
    ASSERT_NOEXCEPT(c_m.stride(r));
  }

  typename E::index_type expected_size = 1;
  for (typename E::rank_type r = 0; r < E::rank(); r++) {
    if (ext.extent(r) == 0) {
      expected_size = 0;
      break;
    }
    expected_size += (ext.extent(r) - 1) * static_cast<typename E::index_type>(strides[r]);
  }
  ASSERT_EQ(m.required_span_size(), expected_size);
  ASSERT_EQ(c_m.required_span_size(), expected_size);
  ASSERT_NOEXCEPT(m.required_span_size());
  ASSERT_NOEXCEPT(c_m.required_span_size());

  PREVIEW_STATIC_ASSERT(std::is_trivially_copyable<M>::value);
  PREVIEW_STATIC_ASSERT(preview::regular<M>::value);
}

TEST(MdSpanLayoutStride, VERSIONED(properties)) {
  constexpr size_t D = preview::dynamic_extent;
  test_layout_mapping_stride(preview::extents<int>(), std::array<int, 0>{}, true);
  test_layout_mapping_stride(preview::extents<signed char, 4, 5>(), std::array<signed char, 2>{1, 4}, true);
  test_layout_mapping_stride(preview::extents<signed char, 4, 5>(), std::array<signed char, 2>{1, 5}, false);
  test_layout_mapping_stride(preview::extents<unsigned, D, 4>(7), std::array<unsigned, 2>{20, 2}, false);
  test_layout_mapping_stride(preview::extents<size_t, D, D, D, D>(3, 3, 3, 3), std::array<size_t, 4>{3, 1, 9, 27}, true);
}
