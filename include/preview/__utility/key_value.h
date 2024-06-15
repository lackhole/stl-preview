//
// Created by YongGyu Lee on 2/22/24.
//

#ifndef PREVIEW_UTILITY_KEY_VALUE_H_
#define PREVIEW_UTILITY_KEY_VALUE_H_

#include <cstddef>
#include <tuple>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__tuple/tuple_like.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace detail {

template<std::size_t I>
struct element_niebloid {
  template<typename T, std::enable_if_t<conjunction<
      tuple_like<T>,
      internal::tuple_size_greater_than<remove_cvref_t<T>, I>
  >::value, int> = 0>
  constexpr decltype(auto) operator()(T&& t) const noexcept {
    return std::get<I>(std::forward<T>(t));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::element_niebloid<0> key;
PREVIEW_INLINE_VARIABLE constexpr detail::element_niebloid<0> first;
PREVIEW_INLINE_VARIABLE constexpr detail::element_niebloid<1> value;
PREVIEW_INLINE_VARIABLE constexpr detail::element_niebloid<1> second;

template<std::size_t I>
PREVIEW_INLINE_VARIABLE constexpr detail::element_niebloid<I> element;

} // namespace preview


#endif // PREVIEW_UTILITY_KEY_VALUE_H_
