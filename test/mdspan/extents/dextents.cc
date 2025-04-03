//
// Created by yonggyulee on 2024. 11. 3.
//
#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "../../test_utils.h"
#include "../print_to.h"

template<typename T>
class MdSpanExtentsDExtents : public testing::Test {};
using MdSpanExtentsDExtentsTypes = ::testing::Types<
    int, unsigned int
>;
TYPED_TEST_SUITE(MdSpanExtentsDExtents, MdSpanExtentsDExtentsTypes);

VERSIONED_TYPE_TEST(MdSpanExtentsDExtents, alias_template_dextents) {
  using IndexType = TypeParam;
  constexpr size_t D = preview::dynamic_extent;

  EXPECT_EQ_TYPE(preview::dims<0, IndexType>, preview::extents<IndexType>);
  EXPECT_EQ_TYPE(preview::dims<1, IndexType>, preview::extents<IndexType, D>);
  EXPECT_EQ_TYPE(preview::dims<2, IndexType>, preview::extents<IndexType, D, D>);
  EXPECT_EQ_TYPE(preview::dims<3, IndexType>, preview::extents<IndexType, D, D, D>);
  EXPECT_EQ_TYPE(preview::dims<9, IndexType>, preview::extents<IndexType, D, D, D, D, D, D, D, D, D>);
}
