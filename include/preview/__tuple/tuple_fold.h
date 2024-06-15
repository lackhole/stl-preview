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

template<std::size_t N, typename Tuple, typename T, typename F>
constexpr auto tuple_fold_left_impl(Tuple&& tuple, T&& x, F&& f, std::integral_constant<std::size_t, N - 1>) {
    return preview::invoke(
        std::forward<F>(f),
        std::forward<T>(x),
        std::get<N - 1>(std::forward<Tuple>(tuple))
    );
}

template<std::size_t N, typename Tuple, typename T, typename F, std::size_t I, std::enable_if_t<(I + 1) != N, int> = 0>
constexpr auto tuple_fold_left_impl(Tuple&& tuple, T&& x, F&& f, std::integral_constant<std::size_t, I>) {
  return tuple_fold_left_impl<N>(
      std::forward<Tuple>(tuple),
      preview::invoke(
          std::forward<F>(f),
          std::forward<T>(x),
          std::get<I>(std::forward<Tuple>(tuple))
      ),
      std::forward<F>(f),
      std::integral_constant<std::size_t, I + 1>{}
  );
}

} // namespace detail

template<typename Tuple, typename T, typename F, std::enable_if_t<conjunction<
    tuple_like<Tuple>
>::value, int> = 0>
constexpr auto tuple_fold_left(Tuple&& tuple, T&& init, F&& f) {
  return detail::tuple_fold_left_impl<std::tuple_size<remove_cvref_t<Tuple>>::value>(
      std::forward<Tuple>(tuple),
      std::forward<T>(init),
      std::forward<F>(f),
      std::integral_constant<std::size_t, 0>{}
  );
}

} // namespace preview

#endif // PREVIEW_TUPLE_TUPLE_FOLD_H_
