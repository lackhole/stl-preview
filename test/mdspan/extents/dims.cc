//
// Created by yonggyulee on 2024. 11. 3.
//
#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "../../test_utils.h"
#include "../print_to.h"

template<typename T>
class MdSpanExtentsDims : public testing::Test {};
using MdSpanExtentsDimsTypes = ::testing::Types<
    int, unsigned int
>;
TYPED_TEST_SUITE(MdSpanExtentsDims, MdSpanExtentsDimsTypes);

VERSIONED_TYPE_TEST(MdSpanExtentsDims, alias_template_dims) {
  using IndexType = TypeParam;
  constexpr size_t D = preview::dynamic_extent;

  EXPECT_EQ_TYPE(preview::dims<0, IndexType>, preview::extents<IndexType>);
  EXPECT_EQ_TYPE(preview::dims<1, IndexType>, preview::extents<IndexType, D>);
  EXPECT_EQ_TYPE(preview::dims<2, IndexType>, preview::extents<IndexType, D, D>);
  EXPECT_EQ_TYPE(preview::dims<3, IndexType>, preview::extents<IndexType, D, D, D>);
  EXPECT_EQ_TYPE(preview::dims<9, IndexType>, preview::extents<IndexType, D, D, D, D, D, D, D, D, D>);
}

TEST(MdSpanExtentsDims, VERSIONED(alias_template_dims_size_t)) {
  constexpr size_t D = preview::dynamic_extent;
  EXPECT_EQ_TYPE(preview::dims<0>, preview::extents<size_t>);
  EXPECT_EQ_TYPE(preview::dims<1>, preview::extents<size_t, D>);
  EXPECT_EQ_TYPE(preview::dims<2>, preview::extents<size_t, D, D>);
  EXPECT_EQ_TYPE(preview::dims<3>, preview::extents<size_t, D, D, D>);
  EXPECT_EQ_TYPE(preview::dims<9>, preview::extents<size_t, D, D, D, D, D, D, D, D, D>);
}
