
#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "gtest.h"
#include "mdspan/print_to.h"

#include "mdspan/CustomTestLayouts.h"

template<typename From, typename FromE>
constexpr From create(const FromE& src_exts, std::true_type) {
  // just construct some strides which aren't layout_left/layout_right
  std::array<size_t, FromE::rank()> strides;
  size_t stride = 2;
  for (size_t r = 0; r < FromE::rank(); r++) {
    strides[r] = stride;
    stride *= src_exts.extent(r);
  }
  return From(src_exts, strides);
}

template<typename From, typename FromE>
constexpr From create(const FromE& src_exts, std::false_type) {
  return From(src_exts);
}

template <bool implicit, class FromL, class ToE, class FromE>
constexpr void test_conversion(FromE src_exts) {
  using To   = preview::layout_stride::mapping<ToE>;
  using From = typename FromL::template mapping<FromE>;


  From src(create<From>(src_exts, std::is_same<FromL, preview::layout_stride>{}));

  ASSERT_NOEXCEPT(To(src));
  To dest(src);
  ASSERT_EQ(dest, src);

#if PREVIEW_CXX_VERSION >= 17
  if constexpr (implicit) {
    To dest_implicit = src;
    ASSERT_EQ(dest_implicit, src);
  } else {
    ASSERT_TRUE((!std::is_convertible<From, To>::value));
  }
#endif
}

template <class FromL, class T1, class T2>
constexpr void test_conversion() {
  constexpr size_t D = preview::dynamic_extent;
  constexpr bool idx_convertible =
      static_cast<size_t>(std::numeric_limits<T1>::max()) >= static_cast<size_t>(std::numeric_limits<T2>::max());
  constexpr bool l_convertible =
      std::is_same<FromL, preview::layout_right>::value || std::is_same<FromL, preview::layout_left>::value ||
      std::is_same<FromL, preview::layout_stride>::value;
  constexpr bool idx_l_convertible = idx_convertible && l_convertible;

  // clang-format off
  // adding extents convertibility expectation
  test_conversion<idx_l_convertible && true,  FromL, preview::extents<T1>>(preview::extents<T2>());
  test_conversion<idx_l_convertible && true,  FromL, preview::extents<T1, D>>(preview::extents<T2, D>(0));
  test_conversion<idx_l_convertible && true,  FromL, preview::extents<T1, D>>(preview::extents<T2, D>(5));
  test_conversion<idx_l_convertible && false, FromL, preview::extents<T1, 5>>(preview::extents<T2, D>(5));
  test_conversion<idx_l_convertible && true,  FromL, preview::extents<T1, 5>>(preview::extents<T2, 5>());
  test_conversion<idx_l_convertible && false, FromL, preview::extents<T1, 5, D>>(preview::extents<T2, D, D>(5, 5));
  test_conversion<idx_l_convertible && true,  FromL, preview::extents<T1, D, D>>(preview::extents<T2, D, D>(5, 5));
  test_conversion<idx_l_convertible && true,  FromL, preview::extents<T1, D, D>>(preview::extents<T2, D, 7>(5));
  test_conversion<idx_l_convertible && true,  FromL, preview::extents<T1, 5, 7>>(preview::extents<T2, 5, 7>());
  test_conversion<idx_l_convertible && false, FromL, preview::extents<T1, 5, D, 8, D, D>>(preview::extents<T2, D, D, 8, 9, 1>(5, 7));
  test_conversion<idx_l_convertible && true,  FromL, preview::extents<T1, D, D, D, D, D>>(
                                                     preview::extents<T2, D, D, D, D, D>(5, 7, 8, 9, 1));
  test_conversion<idx_l_convertible && true,  FromL, preview::extents<T1, D, D, 8, 9, D>>(preview::extents<T2, D, 7, 8, 9, 1>(5));
  test_conversion<idx_l_convertible && true,  FromL, preview::extents<T1, 5, 7, 8, 9, 1>>(preview::extents<T2, 5, 7, 8, 9, 1>());
  // clang-format on
}

template <class IdxT, size_t... Extents>
using ToM = preview::layout_stride::mapping<preview::extents<IdxT, Extents...>>;

template <class FromL, class IdxT, size_t... Extents>
using FromM = typename FromL::template mapping<preview::extents<IdxT, Extents...>>;

template <class FromL>
constexpr void test_no_implicit_conversion() {
  constexpr size_t D = preview::dynamic_extent;

  // Sanity check that one static to dynamic conversion works
  static_assert(std::is_constructible<ToM<int, D>, FromM<FromL, int, 5>>::value, "");
  static_assert(std::is_convertible<FromM<FromL, int, 5>, ToM<int, D>>::value, "");

  // Check that dynamic to static conversion only works explicitly
  static_assert(std::is_constructible<ToM<int, 5>, FromM<FromL, int, D>>::value, "");
  static_assert(!std::is_convertible<FromM<FromL, int, D>, ToM<int, 5>>::value, "");

  // Sanity check that one static to dynamic conversion works
  static_assert(std::is_constructible<ToM<int, D, 7>, FromM<FromL, int, 5, 7>>::value, "");
  static_assert(std::is_convertible<FromM<FromL, int, 5, 7>, ToM<int, D, 7>>::value, "");

  // Check that dynamic to static conversion only works explicitly
  static_assert(std::is_constructible<ToM<int, 5, 7>, FromM<FromL, int, D, 7>>::value, "");
  static_assert(!std::is_convertible<FromM<FromL, int, D, 7>, ToM<int, 5, 7>>::value, "");

  // Sanity check that smaller index_type to larger index_type conversion works
  static_assert(std::is_constructible<ToM<size_t, 5>, FromM<FromL, int, 5>>::value, "");
  static_assert(std::is_convertible<FromM<FromL, int, 5>, ToM<size_t, 5>>::value, "");

  // Check that larger index_type to smaller index_type conversion works explicitly only
  static_assert(std::is_constructible<ToM<int, 5>, FromM<FromL, size_t, 5>>::value, "");
  static_assert(!std::is_convertible<FromM<FromL, size_t, 5>, ToM<int, 5>>::value, "");
}

template <class FromL>
constexpr void test_rank_mismatch() {
  constexpr size_t D = preview::dynamic_extent;

  static_assert(!std::is_constructible<ToM<int, D>, FromM<FromL, int>>::value, "");
  static_assert(!std::is_constructible<ToM<int>, FromM<FromL, int, D, D>>::value, "");
  static_assert(!std::is_constructible<ToM<int, D>, FromM<FromL, int, D, D>>::value, "");
  static_assert(!std::is_constructible<ToM<int, D, D, D>, FromM<FromL, int, D, D>>::value, "");
}

template <class FromL>
constexpr void test_static_extent_mismatch() {
  constexpr size_t D = preview::dynamic_extent;

  static_assert(!std::is_constructible<ToM<int, D, 5>, FromM<FromL, int, D, 4>>::value, "");
  static_assert(!std::is_constructible<ToM<int, 5>, FromM<FromL, int, 4>>::value, "");
  static_assert(!std::is_constructible<ToM<int, 5, D>, FromM<FromL, int, 4, D>>::value, "");
}

template <class FromL>
constexpr void test_layout() {
  test_conversion<FromL, int, int>();
  test_conversion<FromL, int, size_t>();
  test_conversion<FromL, size_t, int>();
  test_conversion<FromL, size_t, long>();
  // the implicit convertibility test doesn't apply to non preview::layouts
#if PREVIEW_CXX_VERSION >= 17
  if constexpr (!std::is_same_v<FromL, always_convertible_layout>)
    test_no_implicit_conversion<FromL>();
#endif
  test_rank_mismatch<FromL>();
  test_static_extent_mismatch<FromL>();
}

TEST(MdSpanLayoutStride, VERSIONED(ctor_strided_mapping)) {
  test_layout<preview::layout_right>();
  test_layout<preview::layout_left>();
  test_layout<preview::layout_stride>();
  test_layout<always_convertible_layout>();
}
