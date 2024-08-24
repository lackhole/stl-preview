//
// Created by YongGyu Lee on 2/13/24.
//

#ifndef PREVIEW_TUPLE_TUPLE_FOLD_H_
#define PREVIEW_TUPLE_TUPLE_FOLD_H_

#include <cstddef>
#include <tuple>
#include <utility>

#include "preview/__functional/invoke.h"
#include "preview/__tuple/tuple_like.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace detail {

template<std::size_t I, std::size_t S, typename Tuple, typename T, typename F, std::enable_if_t<(I == S), int> = 0>
constexpr auto tuple_fold_left_impl(Tuple&&, T&& x, F&&) {
  return std::forward<T>(x);
}

template<std::size_t I, std::size_t S, typename Tuple, typename T, typename F, std::enable_if_t<(I != S), int> = 0>
constexpr auto tuple_fold_left_impl(Tuple&& tuple, T&& x, F&& f) {
  auto&& r = preview::invoke(f, std::forward<T>(x), std::get<I>(std::forward<Tuple>(tuple)));
  return tuple_fold_left_impl<I + 1, S>(
      std::forward<Tuple>(tuple),
      std::forward<decltype(r)>(r),
      std::forward<F>(f)
  );
}

} // namespace detail

template<typename Tuple, typename T, typename F, std::enable_if_t<tuple_like<Tuple>::value, int> = 0>
constexpr auto tuple_fold_left(Tuple&& tuple, T&& init, F&& f) {
  return detail::tuple_fold_left_impl<0, std::tuple_size<remove_cvref_t<Tuple>>::value>(
      std::forward<Tuple>(tuple),
      std::forward<T>(init),
      std::forward<F>(f)
  );
}

} // namespace preview

#endif // PREVIEW_TUPLE_TUPLE_FOLD_H_
