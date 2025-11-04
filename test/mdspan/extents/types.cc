//
// Created by yonggyulee on 2024. 11. 3.
//
#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "preview/core.h"

#include "../../test_utils.h"
#include "../print_to.h"

template<typename T>
class MdSpanExtents : public testing::Test {};
using MdSpanExtentsTypes = ::testing::Types<
    int,
    unsigned,
    signed char,
    long long,
    std::size_t
>;
TYPED_TEST_SUITE(MdSpanExtents, MdSpanExtentsTypes);


template <class E, class IndexType, size_t... Extents>
void testExtents() {
  EXPECT_EQ_TYPE(typename E::index_type, IndexType);
  EXPECT_EQ_TYPE(typename E::size_type, std::make_unsigned_t<IndexType>);
  EXPECT_EQ_TYPE(typename E::rank_type, size_t);

  PREVIEW_STATIC_ASSERT(sizeof...(Extents) == E::rank());
#if PREVIEW_CXX_VERSION >= 17
  PREVIEW_STATIC_ASSERT((static_cast<size_t>(Extents == preview::dynamic_extent) + ...) == E::rank_dynamic());
#endif

  PREVIEW_STATIC_ASSERT(preview::regular<E>::value);
  PREVIEW_STATIC_ASSERT(std::is_trivially_copyable<E>::value);
  PREVIEW_STATIC_ASSERT(std::is_empty<E>::value == (E::rank_dynamic() == 0));
}

template <class IndexType, size_t... Extents>
void testExtents() {
  testExtents<preview::extents<IndexType, Extents...>, IndexType, Extents...>();
}

VERSIONED_TYPE_TEST(MdSpanExtents, Types) {
  using T = TypeParam;
  constexpr std::size_t D = preview::dynamic_extent;
  constexpr size_t S = 5;

  testExtents<T, D>();
  testExtents<T, 3>();
  testExtents<T, 3, 3>();
  testExtents<T, 3, D>();
  testExtents<T, D, 3>();
  testExtents<T, D, D>();
  testExtents<T, 3, 3, 3>();
  testExtents<T, 3, 3, D>();
  testExtents<T, 3, D, D>();
  testExtents<T, D, 3, D>();
  testExtents<T, D, D, D>();
  testExtents<T, 3, D, 3>();
  testExtents<T, D, 3, 3>();
  testExtents<T, D, D, 3>();

  testExtents<T, 9, 8, 7, 6, 5, 4, 3, 2, 1>();
  testExtents<T, 9, D, 7, 6, D, D, 3, D, D>();
  testExtents<T, D, D, D, D, D, D, D, D, D>();
}

