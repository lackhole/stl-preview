//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_EMPTY_H_
#define PREVIEW_RANGES_EMPTY_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/size.h"
#include "preview/__type_traits/detail/return_category.h"
#include "preview/__type_traits/void_t.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

using preview::detail::return_category;

template<typename T, typename = void>
struct empty_member_check : std::false_type {
  using category = return_category<0>;
};
template<typename T>
struct empty_member_check<T, void_t<decltype( bool(std::declval<T>().empty()) )>> : std::true_type {
  using category = return_category<1, bool>;
};

template<typename T, typename = void>
struct empty_size_check : std::false_type {
  using category = return_category<0>;
};
template<typename T>
struct empty_size_check<T, void_t<decltype( ranges::size(std::declval<T>()) == 0 )>> : std::true_type {
  using category = return_category<2, bool>;
};


template<typename I, typename S, bool = rel_ops::is_equality_comparable<I, S>()>
struct empty_range_check_2 : std::false_type {
  using category = return_category<0>;
};
template<typename I, typename S>
struct empty_range_check_2<I, S, true> : forward_iterator<I> {
  using category = return_category<3, bool>;
};

template<typename R, typename I = void, typename S = void>
struct empty_range_check : std::false_type {
  using category = return_category<0>;
};
template<typename R>
struct empty_range_check<
      R,
      void_t<decltype( ranges::begin(std::declval<R>()) )>,
      void_t<decltype( ranges::end  (std::declval<R>()) )>
    >
    : empty_range_check_2<
        /* I = */decltype( ranges::begin(std::declval<R>()) ),
        /* S = */decltype( ranges::end  (std::declval<R>()) )
      > {};

template<typename T>
struct empty_return_category
    : std::conditional_t<
        empty_member_check<T>::value, typename empty_member_check<T>::category,
      std::conditional_t<
        empty_size_check<T>::value, typename empty_size_check<T>::category,
      std::conditional_t<
        empty_range_check<T>::value, typename empty_range_check<T>::category,
        return_category<0>
      >>> {};

template<typename T, typename R>
constexpr R empty_impl(T&& t, return_category<1, R>) {
  return bool(t.empty());
}

template<typename T, typename R>
constexpr R empty_impl(T&& t, return_category<2, R>) {
  return (ranges::size(t) == 0);
}

template<typename T, typename R>
constexpr R empty_impl(T&& t, return_category<3, R>) {
  using namespace preview::rel_ops;
  return bool(ranges::begin(t) == ranges::end(t));
}

struct empty_niebloid {
  template<typename T>
  constexpr typename empty_return_category<T&&>::return_type
  operator()(T&& t) const {
    return detail::empty_impl(std::forward<T>(t), detail::empty_return_category<T&&>{});
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::empty_niebloid empty{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_EMPTY_H_
