//
// Created by yonggyulee on 2024. 11. 3.
//

#include "preview/mdspan.h"
#include "../../test_utils.h"

#include <utility>

#include "../MinimalElementType.h"

template<typename T>
class MdSpanDefaultAccessorCtorDefault : public testing::Test {};
using MdSpanDefaultAccessorCtorDefaultTypes = ::testing::Types<
    int,
    const int,
    MinimalElementType,
    const MinimalElementType
>;
TYPED_TEST_SUITE(MdSpanDefaultAccessorCtorDefault, MdSpanDefaultAccessorCtorDefaultTypes, CVNameGenerator);

VERSIONED_TYPE_TEST(MdSpanDefaultAccessorCtorDefault, ctor_conversion) {
  using T = TypeParam;

  ASSERT_NOEXCEPT(preview::default_accessor<T>{});
  PREVIEW_MAYBE_UNUSED preview::default_accessor<T> acc;
  (void)acc;
  static_assert(std::is_trivially_default_constructible<preview::default_accessor<T>>::value, "");
}
