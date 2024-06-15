//
// Created by YongGyu Lee on 4/9/24.
//

#ifndef PREVIEW_TUPLE_TUPLE_FOR_EACH_H_
#define PREVIEW_TUPLE_TUPLE_FOR_EACH_H_

#include <cstddef>
#include <tuple>
#include <utility>
#include <type_traits>

#include "preview/__functional/invoke.h"
#include "preview/__utility/in_place.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/copy_cvref.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace detail {

template<typename Tuple>
using tuple_index_sequence = std::make_index_sequence<std::tuple_size<remove_cvref_t<Tuple>>::value>;

template<typename Tuple, typename F, typename Seq>
struct tuple_for_each_invocable_impl;

template<typename Tuple, typename F, std::size_t... I>
struct tuple_for_each_invocable_impl<Tuple, F, std::index_sequence<I...>>
    : conjunction<
        is_invocable<F, decltype(std::get<I>(std::declval<Tuple>()))>...
      > {};

template<typename Tuple, typename F>
struct tuple_for_each_invocable
    : tuple_for_each_invocable_impl<Tuple, F, tuple_index_sequence<Tuple>> {};

template<typename Tuple, typename F, std::size_t... I>
constexpr void tuple_for_each_impl(std::index_sequence<I...>, Tuple&& t, F&& f) {
  int dummy[] = {
      (preview::invoke(f, std::get<I>(std::forward<Tuple>(t))), 0)...
  };
  (void)dummy;
}

template<typename Tuple, typename F, typename Seq>
struct tuple_for_each_in_place_index_invocable_impl;

template<typename Tuple, typename F, std::size_t... I>
struct tuple_for_each_in_place_index_invocable_impl<Tuple, F, std::index_sequence<I...>>
    : conjunction<
        is_invocable<F, decltype(std::get<I>(std::declval<Tuple>())), std::integral_constant<std::size_t, I>>...
      > {};

template<typename Tuple, typename F>
struct tuple_for_each_in_place_index_invocable
    : tuple_for_each_in_place_index_invocable_impl<Tuple, F, tuple_index_sequence<Tuple>> {};

template<typename Tuple, typename F, std::size_t... I>
constexpr void tuple_for_each_index_impl(std::index_sequence<I...>, Tuple&& t, F&& f) {
  int dummy[] = {
      (preview::invoke(f, std::get<I>(std::forward<Tuple>(t)), std::integral_constant<std::size_t, I>{}), 0)...
  };
  (void)dummy;
}

} // namespace detail

// Invokes f(std::get<i>(t)) for each i in [0, std::tuple_size<Tuple>)
template<typename Tuple, typename F>
constexpr std::enable_if_t<detail::tuple_for_each_invocable<Tuple, F>::value>
tuple_for_each(Tuple&& t, F&& f) {
  return preview::detail::tuple_for_each_impl(
      detail::tuple_index_sequence<Tuple>{},
      std::forward<Tuple>(t),
      std::forward<F>(f)
  );
}

// Invokes f(std::get<i>(t), std::integral_constant<std::size_t, i>{}) for each i in [0, std::tuple_size<Tuple>)
template<typename Tuple, typename F>
constexpr std::enable_if_t<detail::tuple_for_each_in_place_index_invocable<Tuple, F>::value>
tuple_for_each_index(Tuple&& t, F&& f) {
  return preview::detail::tuple_for_each_index_impl(
      detail::tuple_index_sequence<Tuple>{},
      std::forward<Tuple>(t),
      std::forward<F>(f)
  );
}

} // namespace preview

#endif // PREVIEW_TUPLE_TUPLE_FOR_EACH_H_
