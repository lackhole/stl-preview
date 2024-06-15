//
// Created by YongGyu Lee on 3/25/24.
//

#ifndef PREVIEW_RANGES_RANGE_ADAPTOR_H_
#define PREVIEW_RANGES_RANGE_ADAPTOR_H_

#include <tuple>
#include <type_traits>
#include <utility>

#include "preview/__concepts/different_from.h"
#include "preview/__ranges/range_adaptor_closure.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"

namespace preview {
namespace ranges {

template<typename Niebloid, typename... Args>
class range_adaptor : public range_adaptor_closure<range_adaptor<Niebloid, Args...>> {
 public:
  static_assert(std::is_default_constructible<Niebloid>::value, "Constraints not satisfied");
  static_assert(std::is_empty<Niebloid>::value, "Constraints not satisfied");

  template<typename... T, std::enable_if_t<conjunction<
      different_from_variadic<range_adaptor, T...>,
      bool_constant<(sizeof...(Args) == sizeof...(T))>
  >::value, int> = 0>
  constexpr explicit range_adaptor(T&&... args)
      noexcept(conjunction<std::is_nothrow_constructible<Args, T>...>::value)
      : args_(std::forward<T>(args)...) {}

  template<typename R, std::enable_if_t<is_invocable<Niebloid, R, Args&...>::value, int> = 0>
  constexpr decltype(auto) operator()(R&& r) & {
    return call(*this, std::forward<R>(r), std::index_sequence_for<Args...>{});
  }

  template<typename R, std::enable_if_t<is_invocable<Niebloid, R, const Args&...>::value, int> = 0>
  constexpr decltype(auto) operator()(R&& r) const & {
    return call(*this, std::forward<R>(r), std::index_sequence_for<Args...>{});
  }

  template<typename R, std::enable_if_t<is_invocable<Niebloid, R, Args&&...>::value, int> = 0>
  constexpr decltype(auto) operator()(R&& r) && {
    return call(std::move(*this), std::forward<R>(r), std::index_sequence_for<Args...>{});
  }

  template<typename R, std::enable_if_t<is_invocable<Niebloid, R, const Args&&...>::value, int> = 0>
  constexpr decltype(auto) operator()(R&& r) const && {
    return call(std::move(*this), std::forward<R>(r), std::index_sequence_for<Args...>{});
  }

 private:
  template<typename This, typename R, std::size_t... I>
  static constexpr decltype(auto) call(This&& thiz, R&& r, std::index_sequence<I...>) {
    return Niebloid{}(std::forward<R>(r), std::get<I>(std::forward<This>(thiz).args_)...);
  }

  std::tuple<Args...> args_;
};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_RANGE_ADAPTOR_H_
