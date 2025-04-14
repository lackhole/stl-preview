
#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "../../test_utils.h"
#include "../print_to.h"

// Common requirements of all layout mappings
template <class M, size_t... Idxs>
void test_mapping_requirements(std::index_sequence<Idxs...>) {
  using E = typename M::extents_type;
  static_assert(preview::detail::is_extents<E>::value, "");
  static_assert(std::is_copy_constructible<M>::value, "");
  static_assert(std::is_nothrow_move_constructible<M>::value, "");
  static_assert(std::is_nothrow_move_assignable<M>::value, "");
  static_assert(preview::is_nothrow_swappable<M>::value, "");
  EXPECT_EQ_TYPE(typename M::index_type, typename E::index_type);
  EXPECT_EQ_TYPE(typename M::size_type, typename E::size_type);
  EXPECT_EQ_TYPE(typename M::rank_type, typename E::rank_type);
  EXPECT_EQ_TYPE(typename M::layout_type, preview::layout_stride);
  EXPECT_EQ_TYPE(typename M::layout_type::template mapping<E>, M);
  static_assert(std::is_same<decltype(std::declval<M>().extents()), const E&>::value, "");
  static_assert(std::is_same<decltype(std::declval<M>().strides()), std::array<typename M::index_type, E::rank()>>::value, "");
  static_assert(std::is_same<decltype(std::declval<M>()(Idxs...)), typename M::index_type>::value, "");
  static_assert(std::is_same<decltype(std::declval<M>().required_span_size()), typename M::index_type>::value, "");
  static_assert(std::is_same<decltype(std::declval<M>().is_unique()), bool>::value, "");
  static_assert(std::is_same<decltype(std::declval<M>().is_exhaustive()), bool>::value, "");
  static_assert(std::is_same<decltype(std::declval<M>().is_strided()), bool>::value, "");
  static_assert(std::is_same<decltype(std::declval<M>().stride(0)), typename M::index_type>::value, "");
  static_assert(std::is_same<decltype(M::is_always_unique()), bool>::value, "");
  static_assert(std::is_same<decltype(M::is_always_exhaustive()), bool>::value, "");
  static_assert(std::is_same<decltype(M::is_always_strided()), bool>::value, "");
}

template <class L, class E>
void test_layout_mapping_requirements() {
  using M = typename L::template mapping<E>;
  test_mapping_requirements<M>(std::make_index_sequence<E::rank()>());
}

template <class E>
void test_layout_mapping_stride() {
  test_layout_mapping_requirements<preview::layout_stride, E>();
}

TEST(MdSpanLayoutStride, VERSIONED(static_requirements)) {
  constexpr size_t D = preview::dynamic_extent;
  test_layout_mapping_stride<preview::extents<int>>();
  test_layout_mapping_stride<preview::extents<signed char, 4, 5>>();
  test_layout_mapping_stride<preview::extents<unsigned, D, 4>>();
  test_layout_mapping_stride<preview::extents<size_t, D, D, D, D>>();
}
