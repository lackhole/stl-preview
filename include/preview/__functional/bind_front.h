#ifndef PREVIEW_FUNCTIONAL_BIND_FRONT_H_
#define PREVIEW_FUNCTIONAL_BIND_FRONT_H_

#include <type_traits>
#include <utility>

#include "preview/__core/std_version.h"
#include "preview/__concepts/different_from.h"
#include "preview/__concepts/not_null.h"
#include "preview/__functional/invoke.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__utility/compressed_pair.h"

namespace preview {
namespace detail {

template<typename FD, typename... BoundArgs>
class bind_partial_front {
  using indices = std::index_sequence_for<BoundArgs...>;

 public:
  template<typename F, typename... Args, std::enable_if_t<different_from<F, bind_partial_front>::value, int> = 0>
  constexpr explicit bind_partial_front(F&& f, Args&&... args)
      : pair_(compressed_pair_variadic_construct_divider<1>{},
              std::forward<F>(f),
              std::forward<Args>(args)...) {}

  template<typename... CallArgs, std::enable_if_t<is_invocable<FD&, BoundArgs&..., CallArgs&&...>::value, int> = 0>
  constexpr decltype(auto) operator()(CallArgs&&... call_args) &
      noexcept(is_nothrow_invocable<FD&, BoundArgs&..., CallArgs&&...>::value)
  {
    return call(pair_.first(), pair_.second(), indices{}, std::forward<CallArgs>(call_args)...);
  }

  template<typename... CallArgs, std::enable_if_t<is_invocable<const FD&, const BoundArgs&..., CallArgs&&...>::value, int> = 0>
  constexpr decltype(auto) operator()(CallArgs&&... call_args) const &
      noexcept(is_nothrow_invocable<const FD&, const BoundArgs&..., CallArgs&&...>::value)
  {
    return call(pair_.first(), pair_.second(), indices{}, std::forward<CallArgs>(call_args)...);
  }

  template<typename... CallArgs, std::enable_if_t<is_invocable<FD&&, BoundArgs&&..., CallArgs&&...>::value, int> = 0>
  constexpr decltype(auto) operator()(CallArgs&&... call_args) &&
      noexcept(is_nothrow_invocable<FD&&, BoundArgs&&..., CallArgs&&...>::value)
  {
    return call(std::move(pair_.first()), std::move(pair_.second()), indices{}, std::forward<CallArgs>(call_args)...);
  }

  template<typename... CallArgs, std::enable_if_t<is_invocable<const FD&&, const BoundArgs&&..., CallArgs&&...>::value, int> = 0>
  constexpr decltype(auto) operator()(CallArgs&&... call_args) const &&
      noexcept(is_nothrow_invocable<const FD&&, const BoundArgs&&..., CallArgs&&...>::value)
  {
    return call(std::move(pair_.first()), std::move(pair_.second()), indices{}, std::forward<CallArgs>(call_args)...);
  }

 private:
  template<typename F, typename BoundArgsTuple, std::size_t...I, typename... CallArgs>
  static constexpr decltype(auto) call(F&& f, BoundArgsTuple&& tup, std::index_sequence<I...>, CallArgs&&... call_args)
      noexcept(noexcept(
           preview::invoke(std::forward<F>(f), std::get<I>(std::forward<BoundArgsTuple>(tup))..., std::forward<CallArgs>(call_args)...)
      ))
  {
    return preview::invoke(std::forward<F>(f), std::get<I>(std::forward<BoundArgsTuple>(tup))..., std::forward<CallArgs>(call_args)...);
  }

  compressed_pair<FD, std::tuple<BoundArgs...>> pair_;
};

} // namespace detail

template<typename F, typename... Args, std::enable_if_t<conjunction<
    std::is_constructible<std::decay_t<F>, F>,
    std::is_move_constructible<std::decay_t<F>>,
    std::is_constructible<std::decay_t<Args>, Args>...,
    std::is_move_constructible<std::decay_t<Args>>...
>::value, int> = 0>
constexpr auto bind_front(F&& f, Args&&... args) {
  return detail::bind_partial_front<std::decay_t<F>, std::decay_t<Args>...>{
    std::forward<F>(f),
    std::forward<Args>(args)...
  };
}

#if PREVIEW_CXX_VERSION >= 17
template<auto F, typename... Args, std::enable_if_t<conjunction<
    std::is_constructible<std::decay_t<Args>, Args>...,
    std::is_move_constructible<std::decay_t<Args>>...,
    not_null<F>
>::value, int> = 0>
constexpr auto bind_front(Args&&... args) {
  return detail::bind_partial_front<decltype(F), std::decay_t<Args>...>{
      F,
      std::forward<Args>(args)...
  };
}
#endif

} // namespace preview

#endif // PREVIEW_FUNCTIONAL_BIND_FRONT_H_
