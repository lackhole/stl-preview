//
// Created by YongGyu Lee on 2024. 7. 6.
//

#ifndef PREVIEW_FUNCTIONAL_BIND_PARTIAL_H_
#define PREVIEW_FUNCTIONAL_BIND_PARTIAL_H_

#include <tuple>
#include <utility>

#include "preview/__concepts/different_from.h"
#include "preview/__utility/compressed_pair.h"

namespace preview {

// Derived must implement the followings:
//   bind_invocable
//   bind_nothrow_invocable
//   call()
template<typename Derived, typename FD, typename... BoundArgs>
class bind_partial {
  using indices = std::index_sequence_for<BoundArgs...>;

  template<typename DerivedSelf, typename... CallArgs>
  struct bind_invocable
      : Derived::template bind_invocable<DerivedSelf, CallArgs...> {};

  template<typename DerivedSelf, typename... CallArgs>
  struct bind_nothrow_invocable
      : Derived::template bind_nothrow_invocable<DerivedSelf, CallArgs...> {};

 public:
  template<typename F, typename... Args, std::enable_if_t<different_from<F, bind_partial>::value, int> = 0>
  constexpr explicit bind_partial(F&& f, Args&&... args)
      : pair_(compressed_pair_variadic_construct_divider<1>{},
              std::forward<F>(f),
              std::forward<Args>(args)...) {}

  template<typename... CallArgs, std::enable_if_t<bind_invocable<Derived&, CallArgs&&...>::value, int> = 0>
  constexpr decltype(auto) operator()(CallArgs&&... call_args) &
      noexcept(bind_nothrow_invocable<Derived&, CallArgs&&...>::value)
  {
    return Derived::call(pair_.first(), pair_.second(), indices{}, std::forward<CallArgs>(call_args)...);
  }

  template<typename... CallArgs, std::enable_if_t<bind_invocable<const Derived&, CallArgs&&...>::value, int> = 0>
  constexpr decltype(auto) operator()(CallArgs&&... call_args) const &
      noexcept(bind_nothrow_invocable<const Derived&, CallArgs&&...>::value)
  {
    return Derived::call(pair_.first(), pair_.second(), indices{}, std::forward<CallArgs>(call_args)...);
  }

  template<typename... CallArgs, std::enable_if_t<bind_invocable<Derived&&, CallArgs&&...>::value, int> = 0>
  constexpr decltype(auto) operator()(CallArgs&&... call_args) &&
      noexcept(bind_nothrow_invocable<Derived&&, CallArgs&&...>::value)
  {
    return Derived::call(std::move(pair_.first()), std::move(pair_.second()), indices{}, std::forward<CallArgs>(call_args)...);
  }

  template<typename... CallArgs, std::enable_if_t<bind_invocable<const Derived&&, CallArgs&&...>::value, int> = 0>
  constexpr decltype(auto) operator()(CallArgs&&... call_args) const &&
      noexcept(bind_nothrow_invocable<const Derived&&, CallArgs&&...>::value)
  {
    return Derived::call(std::move(pair_.first()), std::move(pair_.second()), indices{}, std::forward<CallArgs>(call_args)...);
  }

 private:
  compressed_pair<FD, std::tuple<BoundArgs...>> pair_;
};

// const-fn version
// Derived must implement the followings:
//   bind_invocable
//   bind_nothrow_invocable
//   call()
template<typename Derived, typename F, F f, typename... BoundArgs>
class bind_partial_const_fn {
  using indices = std::index_sequence_for<BoundArgs...>;

  template<typename DerivedSelf, typename... CallArgs>
  struct bind_invocable
      : Derived::template bind_invocable<DerivedSelf, CallArgs...> {};

  template<typename DerivedSelf, typename... CallArgs>
  struct bind_nothrow_invocable
      : Derived::template bind_nothrow_invocable<DerivedSelf, CallArgs...> {};

 public:
  template<typename... Args, std::enable_if_t<different_from_variadic<Args..., bind_partial_const_fn>::value, int> = 0>
  constexpr explicit bind_partial_const_fn(Args&&... args)
      : bound_args_(std::forward<Args>(args)...) {}

  template<typename... CallArgs, std::enable_if_t<bind_invocable<Derived&, CallArgs&&...>::value, int> = 0>
  constexpr decltype(auto) operator()(CallArgs&&... call_args) &
      noexcept(bind_nothrow_invocable<Derived&, CallArgs&&...>::value)
  {
    return Derived::call(bound_args_, indices{}, std::forward<CallArgs>(call_args)...);
  }

  template<typename... CallArgs, std::enable_if_t<bind_invocable<const Derived&, CallArgs&&...>::value, int> = 0>
  constexpr decltype(auto) operator()(CallArgs&&... call_args) const &
      noexcept(bind_nothrow_invocable<const Derived&, CallArgs&&...>::value)
  {
    return Derived::call(bound_args_, indices{}, std::forward<CallArgs>(call_args)...);
  }

  template<typename... CallArgs, std::enable_if_t<bind_invocable<Derived&&, CallArgs&&...>::value, int> = 0>
  constexpr decltype(auto) operator()(CallArgs&&... call_args) &&
      noexcept(bind_nothrow_invocable<Derived&&, CallArgs&&...>::value)
  {
    return Derived::call(std::move(bound_args_), indices{}, std::forward<CallArgs>(call_args)...);
  }

  template<typename... CallArgs, std::enable_if_t<bind_invocable<const Derived&&, CallArgs&&...>::value, int> = 0>
  constexpr decltype(auto) operator()(CallArgs&&... call_args) const &&
      noexcept(bind_nothrow_invocable<const Derived&&, CallArgs&&...>::value)
  {
    return Derived::call(std::move(bound_args_), indices{}, std::forward<CallArgs>(call_args)...);
  }

 private:
  std::tuple<BoundArgs...> bound_args_;
};

} // namespace preview

#endif // PREVIEW_FUNCTIONAL_BIND_PARTIAL_H_
