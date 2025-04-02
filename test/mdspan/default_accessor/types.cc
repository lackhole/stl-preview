//
// Created by yonggyulee on 2024. 11. 3.
//

#include "preview/mdspan.h"
#include "../../test_utils.h"

#include <utility>

#include "../MinimalElementType.h"

template<typename T>
class MdSpanDefaultAccessorTypes : public testing::Test {};
using MdSpanDefaultAccessorTypesTypes = ::testing::Types<
    int,
    const int,
    MinimalElementType,
    const MinimalElementType
>;
TYPED_TEST_SUITE(MdSpanDefaultAccessorTypes, MdSpanDefaultAccessorTypesTypes, CVNameGenerator);

VERSIONED_TYPE_TEST(MdSpanDefaultAccessorTypes, ctor_conversion) {
  using T = TypeParam;

  using A = preview::default_accessor<T>;
  EXPECT_EQ_TYPE(typename A::offset_policy, A);
  EXPECT_EQ_TYPE(typename A::element_type, T);
  EXPECT_EQ_TYPE(typename A::reference, T&);
  EXPECT_EQ_TYPE(typename A::data_handle_type, T*);

  EXPECT_TRUE_TYPE(preview::semiregular<A>);
  EXPECT_TRUE_TYPE(std::is_trivially_copyable<A>);

  EXPECT_TRUE_TYPE(std::is_empty<A>);
}
