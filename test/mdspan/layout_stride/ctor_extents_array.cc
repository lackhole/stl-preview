//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.

#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "gtest.h"
#include "mdspan/print_to.h"
#include "mdspan/ConvertibleToIntegral.h"

template <class E, class S>
constexpr void test_construction(E e, S s) {
  using M = preview::layout_stride::mapping<E>;
  ASSERT_NOEXCEPT(M{e, s});
  M m(e, s);

  // check correct extents are returned
  ASSERT_NOEXCEPT(m.extents());
  EXPECT_EQ(m.extents(), e);

  // check required_span_size()
  typename E::index_type expected_size = 1;
  for (typename E::rank_type r = 0; r < E::rank(); r++) {
    if (e.extent(r) == 0) {
      expected_size = 0;
      break;
    }
    expected_size += (e.extent(r) - 1) * static_cast<typename E::index_type>(s[r]);
  }
  EXPECT_EQ(m.required_span_size(), expected_size);

  // check strides: node stride function is constrained on rank>0, e.extent(r) is not
  auto strides = m.strides();
  ASSERT_NOEXCEPT(m.strides());
#if PREVIEW_CXX_VERSION >= 17
  if constexpr (E::rank() > 0) {
    for (typename E::rank_type r = 0; r < E::rank(); r++) {
      ASSERT_EQ(m.stride(r), static_cast<typename E::index_type>(s[r]));
      ASSERT_EQ(strides[r], m.stride(r));
    }
  }
#endif
}

TEST(MdSpanLayoutStride, VERSIONED(ctor_extents_array)) {
  constexpr size_t D = preview::dynamic_extent;
  {
    std::array<int, 0> s{};
    test_construction(preview::extents<int>(), s);
  }
  {
    std::array<int, 1> s{1};
    test_construction(preview::extents<unsigned, D>(7), s);
  }
  {
    std::array<int, 1> s{1};
    test_construction(preview::extents<unsigned, D>(0), s);
  }
  {
    std::array<int, 1> s{2};
    test_construction(preview::extents<unsigned, 7>(), s);
  }
  {
    std::array<IntType, 1> s{1};
    test_construction(preview::extents<int, D>(7), s);
  }
  {
    std::array<int, 2> s{3, 30};
    test_construction(preview::extents<unsigned, 7, 8>(), s);
  }
  {
    std::array<int, 4> s{20, 2, 200, 2000};
    test_construction(preview::extents<int64_t, D, 8, D, D>(7, 9, 10), s);
    test_construction(preview::extents<int64_t, D, 8, D, D>(0, 9, 10), s);
    test_construction(preview::extents<int64_t, D, 8, D, D>(0, 8, 0), s);
  }
  {
    std::array<int, 4> s{200, 20, 20, 2000};
    test_construction(preview::extents<int64_t, D, D, D, D>(7, 0, 8, 9), s);
    test_construction(preview::extents<int64_t, D, D, D, D>(7, 8, 0, 9), s);
    test_construction(preview::extents<int64_t, D, D, D, D>(7, 1, 8, 9), s);
    test_construction(preview::extents<int64_t, D, D, D, D>(7, 8, 1, 9), s);
    test_construction(preview::extents<int64_t, D, D, D, D>(7, 1, 1, 9), s);
    test_construction(preview::extents<int64_t, D, D, D, D>(7, 0, 0, 9), s);
    test_construction(preview::extents<int64_t, D, D, D, D>(7, 1, 1, 9), s);
    test_construction(preview::extents<int64_t, D, D, D, D>(7, 1, 0, 9), s);
    test_construction(preview::extents<int64_t, D, D, D, D>(7, 0, 1, 9), s);
  }

  {
    using mapping_t = preview::layout_stride::mapping<preview::dextents<unsigned, 2>>;
    // wrong strides size
    static_assert(!std::is_constructible<mapping_t, preview::dextents<int, 2>, std::array<int, 3>>::value, "");
    static_assert(!std::is_constructible<mapping_t, preview::dextents<int, 2>, std::array<int, 1>>::value, "");
    // wrong extents rank
    static_assert(!std::is_constructible<mapping_t, preview::dextents<int, 3>, std::array<int, 2>>::value, "");
    // none-convertible strides
    static_assert(!std::is_constructible<mapping_t, preview::dextents<int, 2>, std::array<IntType, 2>>::value, "");
  }
  {
    // not no-throw constructible index_type from stride
    using mapping_t = preview::layout_stride::mapping<preview::dextents<unsigned char, 2>>;
    static_assert(std::is_convertible<IntType, unsigned char>::value, "");
    static_assert(!std::is_constructible<mapping_t, preview::dextents<int, 2>, std::array<IntType, 2>>::value, "");
  }
}
