//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_BEGIN_H_
#define PREVIEW_RANGES_BEGIN_H_

#include <cstddef>
#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/input_or_output_iterator.h"
#include "preview/__type_traits/detail/return_category.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__core/decay_copy.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/is_complete.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

using preview::detail::return_category;

template<typename T, bool = std::is_array<remove_cvref_t<T>>::value>
struct begin_array_check : std::false_type {
  using category = return_category<0>;
};
template<typename T>
struct begin_array_check<T, true> : std::true_type {
  using category = return_category<1, decltype(std::declval<T>() + 0)>;
};

template<typename T, typename = void>
struct begin_member_check : std::false_type {
  using category = return_category<0>;
};
template<typename T>
struct begin_member_check<T, void_t<decltype(preview_decay_copy(std::declval<T>().begin()))>>
    : input_or_output_iterator<decltype(preview_decay_copy(std::declval<T>().begin()))> {
  using category = return_category<2, decltype(preview_decay_copy(std::declval<T>().begin()))>;
};

template<typename T, typename = void>
struct begin_global_check : std::false_type {
  using category = return_category<0>;
};
template<typename T>
struct begin_global_check<T, void_t<decltype(preview_decay_copy(begin(std::declval<T>())))>>
    : input_or_output_iterator<decltype(preview_decay_copy(begin(std::declval<T>())))> {
  using category = return_category<3, decltype(preview_decay_copy(begin(std::declval<T>())))>;
};

template<typename T, bool = begin_member_check<T>::value /* false */>
struct begin_category_impl_2 : begin_global_check<T> {};
template<typename T>
struct begin_category_impl_2<T, true> : begin_member_check<T> {};

template<typename T, bool = begin_array_check<T>::value /* false */>
struct begin_category_impl : begin_category_impl_2<T> {};
template<typename T>
struct begin_category_impl<T, true> : begin_array_check<T> {};

template<typename T>
struct begin_category
    : std::conditional_t<
        disjunction<
          std::is_lvalue_reference<T>,
          enable_borrowed_range_t<remove_cvref_t<T>>
        >::value,
        typename begin_category_impl<T>::category,
        return_category<0>
      > {};

template<typename T, typename R>
constexpr R ranges_begin(T&& t, return_category<1, R>) {
  static_assert(is_complete<std::remove_all_extents_t<std::remove_reference_t<T>>>::value, "Array element must be complete type");
  return t + 0;
}

template<typename T, typename R>
constexpr R ranges_begin(T&& t, return_category<2, R>) {
  return preview_decay_copy(t.begin());
}

template<typename T, typename R>
constexpr R ranges_begin(T&& t, return_category<3, R>) {
  return preview_decay_copy(begin(t));
}

struct begin_niebloid {
  template<typename T>
  constexpr typename begin_category<T&&>::return_type
  operator()(T&& t) const {
    return ranges_begin(std::forward<T>(t), detail::begin_category<T&&>{});
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::begin_niebloid begin{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_BEGIN_H_
