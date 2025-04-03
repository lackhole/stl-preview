//
// Created by YongGyu Lee on 2024. 5. 17.
//

#ifndef PREVIEW_UTILITY_SEQUENCE_FOR_EACH_H_
#define PREVIEW_UTILITY_SEQUENCE_FOR_EACH_H_

#include <cstddef>
#include <type_traits>
#include <utility>

#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"

namespace preview {
namespace detail {

template<typename Seq, typename F, typename... Args>
struct sequence_for_each_invocable : std::false_type {};

template<typename T, T... v, typename F, typename... Args>
struct sequence_for_each_invocable<std::integer_sequence<T, v...>, F, Args...>
    : conjunction<
          is_invocable<F, std::integral_constant<T, v>, Args...>...
      > {};

template<typename T, T... v, typename F, typename... Args, std::enable_if_t<(sizeof...(v) > 0), int> = 0>
constexpr void sequence_for_each_impl(std::integer_sequence<T, v...>, F&& f, Args&&... args) {
  int dummy[] = {
      (preview::invoke(f, std::integral_constant<T, v>{}, std::forward<Args>(args)...), 0)...
  };
  (void)dummy;
}

template<typename T, typename F, typename... Args>
constexpr void sequence_for_each_impl(std::integer_sequence<T>, F&&, Args&&...) {
  // no-op
}

} // namespace detail


// performs f(std::integral_constant<T, i>{}, args...) for i in v...
template<typename T, T... v, typename F, typename... Args>
constexpr std::enable_if_t<detail::sequence_for_each_invocable<std::integer_sequence<T, v...>, F, Args...>::value>
sequence_for_each(std::integer_sequence<T, v...>, F&& f, Args&&... args)
    noexcept(conjunction<
        bool_constant<noexcept(preview::invoke(f, std::integral_constant<T, v>{}, std::forward<Args>(args)...))>...
    >::value)
{
  preview::detail::sequence_for_each_impl(std::integer_sequence<T, v...>{}, std::forward<F>(f), std::forward<Args>(args)...);
}

// performs f(std::integral_constant<T, i>{}, args...) for i in [0, N)
template<typename T, T N, typename F, typename... Args>
constexpr std::enable_if_t<detail::sequence_for_each_invocable<std::make_integer_sequence<T, N>, F>::value>
sequence_for_each(F&& f, Args&&... args)
    noexcept(noexcept(
        preview::sequence_for_each(
            std::make_integer_sequence<T, N>{},
            std::forward<F>(f),
            std::forward<Args>(args)...
        )
    ))
{
  return preview::sequence_for_each(
      std::make_integer_sequence<T, N>{},
      std::forward<F>(f),
      std::forward<Args>(args)...
  );
}

// performs f(std::integral_constant<std::size_t, i>{}, args...) for i in [0, N)
template<std::size_t N, typename F, typename... Args>
constexpr std::enable_if_t<detail::sequence_for_each_invocable<std::make_index_sequence<N>, F, Args...>::value>
sequence_for_each(F&& f, Args&&... args)
    noexcept(noexcept(
        preview::sequence_for_each(
            std::make_index_sequence<N>{},
            std::forward<F>(f),
            std::forward<Args>(args)...
        )
    ))
{
  return preview::sequence_for_each(
      std::make_index_sequence<N>{},
      std::forward<F>(f),
      std::forward<Args>(args)...
  );
}

} // namespace preview

#endif // PREVIEW_UTILITY_SEQUENCE_FOR_EACH_H_
