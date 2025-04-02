//
// Created by yonggyulee on 2024. 11. 3.
//

#include "preview/mdspan.h"
#include "../../test_utils.h"

#include <utility>

#include "../MinimalElementType.h"

template<typename T>
class MdSpanDefaultAccessorCtor : public testing::Test {};
using MdSpanDefaultAccessorCtorTypes = ::testing::Types<
    std::pair<int, int>,
    std::pair<int, const int>,
    std::pair<const int, const int>,
    std::pair<MinimalElementType, MinimalElementType>,
    std::pair<MinimalElementType, const MinimalElementType>,
    std::pair<const MinimalElementType, const MinimalElementType>
>;
TYPED_TEST_SUITE(MdSpanDefaultAccessorCtor, MdSpanDefaultAccessorCtorTypes, CVNameGenerator);

VERSIONED_TYPE_TEST(MdSpanDefaultAccessorCtor, ctor_conversion) {
  using FromT = typename TypeParam::first_type;
  using ToT = typename TypeParam::second_type;

  preview::default_accessor<FromT> acc_from;
  ASSERT_NOEXCEPT(preview::default_accessor<ToT>(acc_from));
  [[maybe_unused]] preview::default_accessor<ToT> acc_to(acc_from);
}

struct Base {};
struct Derived: public Base {};

TEST(VERSIONED(MdSpanDefaultAccessorCtor), conversion) {
  // char is convertible to int, but accessors are not
  static_assert(!std::is_constructible<preview::default_accessor<int>, preview::default_accessor<char>>::value, "");
  // don't allow conversion from const elements to non-const
  static_assert(!std::is_constructible<preview::default_accessor<int>, preview::default_accessor<const int>>::value, "");
  // MinimalElementType is constructible from int, but accessors should not be convertible
  static_assert(!std::is_constructible<preview::default_accessor<MinimalElementType>, preview::default_accessor<int>>::value, "");
  // don't allow conversion from const elements to non-const
  static_assert(!std::is_constructible<preview::default_accessor<MinimalElementType>, preview::default_accessor<const MinimalElementType>>::value, "");
  // don't allow conversion from Base to Derived
  static_assert(!std::is_constructible<preview::default_accessor<Derived>, preview::default_accessor<Base>>::value, "");
  // don't allow conversion from Derived to Base
  static_assert(!std::is_constructible<preview::default_accessor<Base>, preview::default_accessor<Derived>>::value, "");
}

TEST(VERSIONED(MdSpanDefaultAccessorCtor), defaults) {
  // char is convertible to int, but accessors are not
  static_assert(!std::is_constructible<preview::default_accessor<int>, preview::default_accessor<char>>::value, "");
  // don't allow conversion from const elements to non-const
  static_assert(!std::is_constructible<preview::default_accessor<int>, preview::default_accessor<const int>>::value, "");
  // MinimalElementType is constructible from int, but accessors should not be convertible
  static_assert(!std::is_constructible<preview::default_accessor<MinimalElementType>, preview::default_accessor<int>>::value, "");
  // don't allow conversion from const elements to non-const
  static_assert(!std::is_constructible<preview::default_accessor<MinimalElementType>, preview::default_accessor<const MinimalElementType>>::value, "");
  // don't allow conversion from Base to Derived
  static_assert(!std::is_constructible<preview::default_accessor<Derived>, preview::default_accessor<Base>>::value, "");
  // don't allow conversion from Derived to Base
  static_assert(!std::is_constructible<preview::default_accessor<Base>, preview::default_accessor<Derived>>::value, "");
}
