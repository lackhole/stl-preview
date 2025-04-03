//
// Created by yonggyulee on 2024. 11. 6.
//

#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "preview/core.h"

#include "../../test_utils.h"
#include "../print_to.h"

template<typename T>
class MdSpanLayoutLeft : public testing::Test {};
using MdSpanLayoutLeftTypes = ::testing::Types<
    std::pair<int, int>,
    std::pair<int, std::size_t>,
    std::pair<std::size_t, int>,
    std::pair<std::size_t, long>
>;
TYPED_TEST_SUITE(MdSpanLayoutLeft, MdSpanLayoutLeftTypes);


template <class To, class From>
constexpr void test_comparison(bool equal, To dest_exts, From src_exts) {
  preview::layout_left::mapping<To> dest(dest_exts);
  preview::layout_left::mapping<From> src(src_exts);

  ASSERT_NOEXCEPT(dest == src);
  EXPECT_EQ((dest == src), equal);
  EXPECT_EQ((src == dest), equal);
  EXPECT_EQ((dest != src), !equal);
  EXPECT_EQ((src != dest), !equal);
}

struct X {
  constexpr bool does_not_match() { return true; }
};

constexpr X compare_layout_mappings(...) { return {}; }

template <class E1, class E2>
constexpr auto compare_layout_mappings(E1 e1, E2 e2)
  -> decltype(preview::layout_left::mapping<E1>(e1) == preview::layout_left::mapping<E2>(e2)) {
  return true;
}

template <class T1, class T2>
constexpr void test_comparison_different_rank() {
  constexpr size_t D = preview::dynamic_extent;

  // sanity check same rank
  PREVIEW_STATIC_ASSERT(compare_layout_mappings(preview::extents<T1, D>(5), preview::extents<T2, D>(5)));
  PREVIEW_STATIC_ASSERT(compare_layout_mappings(preview::extents<T1, 5>(), preview::extents<T2, D>(5)));
  PREVIEW_STATIC_ASSERT(compare_layout_mappings(preview::extents<T1, D>(5), preview::extents<T2, 5>()));
  PREVIEW_STATIC_ASSERT(compare_layout_mappings(preview::extents<T1, 5>(), preview::extents<T2, 5>()));

  // not equality comparable when rank is not the same
  PREVIEW_STATIC_ASSERT(compare_layout_mappings(preview::extents<T1>(), preview::extents<T2, D>(1)).does_not_match());
  PREVIEW_STATIC_ASSERT(compare_layout_mappings(preview::extents<T1>(), preview::extents<T2, 1>()).does_not_match());

  PREVIEW_STATIC_ASSERT(compare_layout_mappings(preview::extents<T1, D>(1), preview::extents<T2>()).does_not_match());
  PREVIEW_STATIC_ASSERT(compare_layout_mappings(preview::extents<T1, 1>(), preview::extents<T2>()).does_not_match());

  PREVIEW_STATIC_ASSERT(compare_layout_mappings(preview::extents<T1, D>(5), preview::extents<T2, D, D>(5, 5)).does_not_match());
  PREVIEW_STATIC_ASSERT(compare_layout_mappings(preview::extents<T1, 5>(), preview::extents<T2, 5, D>(5)).does_not_match());
  PREVIEW_STATIC_ASSERT(compare_layout_mappings(preview::extents<T1, 5>(), preview::extents<T2, 5, 1>()).does_not_match());

  PREVIEW_STATIC_ASSERT(compare_layout_mappings(preview::extents<T1, D, D>(5, 5), preview::extents<T2, D>(5)).does_not_match());
  PREVIEW_STATIC_ASSERT(compare_layout_mappings(preview::extents<T1, 5, D>(5), preview::extents<T2, D>(5)).does_not_match());
  PREVIEW_STATIC_ASSERT(compare_layout_mappings(preview::extents<T1, 5, 5>(), preview::extents<T2, 5>()).does_not_match());
}

template <class T1, class T2>
constexpr void test_comparison_same_rank() {
  constexpr size_t D = preview::dynamic_extent;

  test_comparison(true, preview::extents<T1>(), preview::extents<T2>());

  test_comparison(true, preview::extents<T1, D>(5), preview::extents<T2, D>(5));
  test_comparison(true, preview::extents<T1, 5>(), preview::extents<T2, D>(5));
  test_comparison(true, preview::extents<T1, D>(5), preview::extents<T2, 5>());
  test_comparison(true, preview::extents<T1, 5>(), preview::extents< T2, 5>());
  test_comparison(false, preview::extents<T1, D>(5), preview::extents<T2, D>(7));
  test_comparison(false, preview::extents<T1, 5>(), preview::extents<T2, D>(7));
  test_comparison(false, preview::extents<T1, D>(5), preview::extents<T2, 7>());
  test_comparison(false, preview::extents<T1, 5>(), preview::extents<T2, 7>());

  test_comparison(true, preview::extents<T1, D, D, D, D, D>(5, 6, 7, 8, 9), preview::extents<T2, D, D, D, D, D>(5, 6, 7, 8, 9));
  test_comparison(true, preview::extents<T1, D, 6, D, 8, D>(5, 7, 9), preview::extents<T2, 5, D, D, 8, 9>(6, 7));
  test_comparison(true, preview::extents<T1, 5, 6, 7, 8, 9>(5, 6, 7, 8, 9), preview::extents<T2, 5, 6, 7, 8, 9>());
  test_comparison(
      false, preview::extents<T1, D, D, D, D, D>(5, 6, 7, 8, 9), preview::extents<T2, D, D, D, D, D>(5, 6, 3, 8, 9));
  test_comparison(false, preview::extents<T1, D, 6, D, 8, D>(5, 7, 9), preview::extents<T2, 5, D, D, 3, 9>(6, 7));
  test_comparison(false, preview::extents<T1, 5, 6, 7, 8, 9>(5, 6, 7, 8, 9), preview::extents<T2, 5, 6, 7, 3, 9>());
}

VERSIONED_TYPE_TEST(MdSpanLayoutLeft, comparison) {
  constexpr std::size_t D = preview::dynamic_extent;
  using T1 = typename TypeParam::first_type;
  using T2 = typename TypeParam::second_type;

  test_comparison_same_rank<T1, T2>();
  test_comparison_different_rank<T1, T2>();
}
