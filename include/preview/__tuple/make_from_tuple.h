//
// Created by yonggyulee on 2023/10/27.
//

#ifndef PREVIEW_TUPLE_MAKE_FROM_TUPLE_H_
#define PREVIEW_TUPLE_MAKE_FROM_TUPLE_H_

#include <cstddef>
#include <type_traits>
#include <utility>

#include "preview/__tuple/tuple_integer_sequence.h"
#include "preview/__tuple/tuple_like.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace detail {

template<typename T, typename Tuple, std::size_t... I>
constexpr T make_from_tuple_impl(Tuple&& t, std::index_sequence<I...>) {
  return T(std::get<I>(std::forward<Tuple>(t))...);
}

template<typename T, typename Tuple, typename IndexSequence>
struct is_constructible_from_tuple_impl;

template<typename T, typename Tuple, std::size_t... I>
struct is_constructible_from_tuple_impl<T, Tuple, std::index_sequence<I...>>
    : std::is_constructible<T, decltype(std::get<I>(std::declval<Tuple>()))...> {};

template<typename T, typename Tuple>
struct is_constructible_from_tuple
    : is_constructible_from_tuple_impl<T, Tuple, tuple_index_sequence<Tuple>> {};

} // namespace detail

template<typename T, typename Tuple, std::enable_if_t<conjunction<
    tuple_like<Tuple>,
    detail::is_constructible_from_tuple<T, Tuple>
>::value, int> = 0>
constexpr T make_from_tuple(Tuple&& t) {
  return preview::detail::make_from_tuple_impl<T>(
      std::forward<Tuple>(t),
      tuple_index_sequence<Tuple>{}
  );
}

} // namespace preview

#endif // PREVIEW_TUPLE_MAKE_FROM_TUPLE_H_
