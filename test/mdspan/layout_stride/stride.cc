
#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "preview/core.h"

#include "../../test_utils.h"
#include "../print_to.h"

template <class E, class... Args>
constexpr void test_stride(std::array<typename E::index_type, E::rank()> strides, Args... args) {
  using M = preview::layout_stride::mapping<E>;
  M m(E(args...), strides);

  ASSERT_NOEXCEPT(m.stride(0));
  for (size_t r = 0; r < E::rank(); r++)
    ASSERT_EQ(strides[r], m.stride(r));

  ASSERT_NOEXCEPT(m.strides());
  auto strides_out = m.strides();
  PREVIEW_STATIC_ASSERT(std::is_same<decltype(strides_out), std::array<typename E::index_type, E::rank()>>::value);
  for (size_t r = 0; r < E::rank(); r++)
    ASSERT_EQ(strides[r], strides_out[r]);
}

TEST(MdSpanLayoutStride, VERSIONED(stride)) {
  constexpr size_t D = preview::dynamic_extent;
  test_stride<preview::extents<unsigned, D>>(std::array<unsigned, 1>{1}, 7);
  test_stride<preview::extents<unsigned, 7>>(std::array<unsigned, 1>{1});
  test_stride<preview::extents<unsigned, 7, 8>>(std::array<unsigned, 2>{8, 1});
  test_stride<preview::extents<int64_t, D, 8, D, D>>(std::array<int64_t, 4>{720, 90, 10, 1}, 7, 9, 10);
}
