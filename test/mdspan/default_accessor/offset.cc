//
// Created by yonggyulee on 2024. 11. 3.
//

#include "preview/mdspan.h"
#include "gtest.h"

#include <utility>

#include "../MinimalElementType.h"

template<typename T>
class MdSpanDefaultAccessorOffset : public testing::Test {};
using MdSpanDefaultAccessorOffsetTypes = ::testing::Types<
    int,
    const int,
    MinimalElementType,
    const MinimalElementType
>;
TYPED_TEST_SUITE(MdSpanDefaultAccessorOffset, MdSpanDefaultAccessorOffsetTypes, CVNameGenerator);

VERSIONED_TYPE_TEST(MdSpanDefaultAccessorOffset, ctor_conversion) {
  using T = TypeParam;

  ElementPool<std::remove_const_t<T>, 10> data;
  T* ptr = data.get_ptr();
  preview::default_accessor<T> acc;
  for(int i = 0; i < 10; i++) {
    static_assert(std::is_same<decltype(acc.offset(ptr, i)), typename preview::default_accessor<T>::data_handle_type>::value, "");
    ASSERT_NOEXCEPT(acc.offset(ptr, i));
    EXPECT_EQ(acc.offset(ptr, i), ptr + i);
  }
}
