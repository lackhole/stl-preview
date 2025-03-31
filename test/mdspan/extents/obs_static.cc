//
// Created by yonggyulee on 2024. 11. 3.
//
#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "gtest.h"
#include "../print_to.h"

template<typename T>
class MdSpanExtentsObsStatic : public testing::Test {};
using MdSpanExtentsObsStaticTypes = ::testing::Types<
    int,
    unsigned,
    signed char,
    long long,
    std::size_t
>;
TYPED_TEST_SUITE(MdSpanExtentsObsStatic, MdSpanExtentsObsStaticTypes);


template <class E, std::size_t rank, std::size_t rank_dynamic, std::size_t... StaticExts, std::size_t... Indices>
void test_static_observers(std::index_sequence<StaticExts...>, std::index_sequence<Indices...>) {
  ASSERT_NOEXCEPT(E::rank());
  static_assert(E::rank() == rank, "");
  ASSERT_NOEXCEPT(E::rank_dynamic());
  static_assert(E::rank_dynamic() == rank_dynamic, "");

  // Let's only test this if the call isn't a precondition violation
  if PREVIEW_CONSTEXPR_AFTER_CXX17 (rank > 0) {
    ASSERT_NOEXCEPT(E::static_extent(0));
    EXPECT_EQ_TYPE(decltype(E::static_extent(0)), std::size_t);
    static_assert(preview::conjunction_v<preview::bool_constant<(E::static_extent(Indices) == StaticExts)>...>, "");
  }
}

template <class E, std::size_t rank, std::size_t rank_dynamic, std::size_t... StaticExts>
void test_static_observers() {
  test_static_observers<E, rank, rank_dynamic>(
      std::index_sequence<StaticExts...>(), std::make_index_sequence<sizeof...(StaticExts)>());
}

VERSIONED_TYPE_TEST(MdSpanExtentsObsStatic, test) {
  using T = TypeParam;
  constexpr std::size_t D = preview::dynamic_extent;
  constexpr size_t S = 5;

  test_static_observers<preview::extents<T>, 0, 0>();

  test_static_observers<preview::extents<T, S>, 1, 0, S>();
  test_static_observers<preview::extents<T, D>, 1, 1, D>();

  test_static_observers<preview::extents<T, S, S>, 2, 0, S, S>();
  test_static_observers<preview::extents<T, S, D>, 2, 1, S, D>();
  test_static_observers<preview::extents<T, D, S>, 2, 1, D, S>();
  test_static_observers<preview::extents<T, D, D>, 2, 2, D, D>();

  test_static_observers<preview::extents<T, S, S, S>, 3, 0, S, S, S>();
  test_static_observers<preview::extents<T, S, S, D>, 3, 1, S, S, D>();
  test_static_observers<preview::extents<T, S, D, S>, 3, 1, S, D, S>();
  test_static_observers<preview::extents<T, D, S, S>, 3, 1, D, S, S>();
  test_static_observers<preview::extents<T, S, D, D>, 3, 2, S, D, D>();
  test_static_observers<preview::extents<T, D, S, D>, 3, 2, D, S, D>();
  test_static_observers<preview::extents<T, D, D, S>, 3, 2, D, D, S>();
  test_static_observers<preview::extents<T, D, D, D>, 3, 3, D, D, D>();
}

