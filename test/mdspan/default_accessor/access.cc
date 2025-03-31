//
// Created by yonggyulee on 2024. 10. 28.
//

#include "preview/mdspan.h"
#include "gtest.h"

#include <limits>

#include "../MinimalElementType.h"

template<typename T>
class MdSpanDefaultAccessorAccess : public testing::Test {};
using MdSpanDefaultAccessorAccessTypes = ::testing::Types<
    int,
    const int,
    MinimalElementType,
    const MinimalElementType>;
TYPED_TEST_SUITE(MdSpanDefaultAccessorAccess, MdSpanDefaultAccessorAccessTypes, CVNameGenerator);

VERSIONED_TYPE_TEST(MdSpanDefaultAccessorAccess, access) {
  using T = TypeParam;

  ElementPool<std::remove_const_t<T>, 10> data;
  T* ptr = data.get_ptr();
  preview::default_accessor<T> acc;
  for(int i = 0; i < 10; i++) {
    static_assert(std::is_same<decltype(acc.access(ptr, i)), typename preview::default_accessor<T>::reference>::value, "");
    ASSERT_NOEXCEPT(acc.access(ptr, i));
    EXPECT_EQ(&acc.access(ptr, i), ptr + i);
  }
}

