#ifndef PREVIEW_FUNCTIONAL_BIND_FRONT_H_
#define PREVIEW_FUNCTIONAL_BIND_FRONT_H_

#include <type_traits>
#include <utility>

#include "preview/__core/cxx_version.h"
#include "preview/__concepts/not_null.h"
#include "preview/__functional/bind_partial.h"
#include "preview/__functional/invoke.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__utility/forward_like.h"

namespace preview {
namespace detail {

template<typename FD, typename... BoundArgs>
class bind_partial_front : bind_partial<bind_partial_front<FD, BoundArgs...>, FD, BoundArgs...> {
  using base = bind_partial<bind_partial_front<FD, BoundArgs...>, FD, BoundArgs...>;
  friend class bind_partial<bind_partial_front<FD, BoundArgs...>, FD, BoundArgs...>;

  template<typename Self, typename... CallArgs>
  struct bind_invocable
      : is_invocable<forward_like_t<Self, FD>, forward_like_t<Self, BoundArgs>..., CallArgs...> {};

  template<typename Self, typename... CallArgs>
  struct bind_nothrow_invocable
      : is_nothrow_invocable<forward_like_t<Self, FD>, forward_like_t<Self, BoundArgs>..., CallArgs...> {};

  template<typename F, typename BoundArgsTuple, std::size_t...I, typename... CallArgs>
  static constexpr decltype(auto) call(F&& f, BoundArgsTuple&& tup, std::index_sequence<I...>, CallArgs&&... call_args)
      noexcept(noexcept(
           preview::invoke(std::forward<F>(f), std::get<I>(std::forward<BoundArgsTuple>(tup))..., std::forward<CallArgs>(call_args)...)
      ))
  {
    return preview::invoke(std::forward<F>(f), std::get<I>(std::forward<BoundArgsTuple>(tup))..., std::forward<CallArgs>(call_args)...);
  }

 public:
  using base::base;
  using base::operator();
};

template<typename F, F f, typename... BoundArgs>
class bind_partial_front_const_fn
    : bind_partial_const_fn<bind_partial_front_const_fn<F, f, BoundArgs...>, F, f, BoundArgs...>
{
  using base = bind_partial_const_fn<bind_partial_front_const_fn<F, f, BoundArgs...>, F, f, BoundArgs...>;
  friend class bind_partial_const_fn<bind_partial_front_const_fn<F, f, BoundArgs...>, F, f, BoundArgs...>;

  template<typename Self, typename... CallArgs>
  struct bind_invocable
      : is_invocable<F, copy_cvref_t<Self, BoundArgs>..., CallArgs...> {};

  template<typename Self, typename... CallArgs>
  struct bind_nothrow_invocable
      : is_nothrow_invocable<F, copy_cvref_t<Self, BoundArgs>..., CallArgs...> {};

  template<typename BoundArgsTuple, std::size_t...I, typename... CallArgs>
  static constexpr decltype(auto) call(BoundArgsTuple&& tup, std::index_sequence<I...>, CallArgs&&... call_args)
      noexcept(noexcept(
           preview::invoke(f, std::get<I>(std::forward<BoundArgsTuple>(tup))..., std::forward<CallArgs>(call_args)...)
      ))
  {
    return preview::invoke(f, std::get<I>(std::forward<BoundArgsTuple>(tup))..., std::forward<CallArgs>(call_args)...);
  }

 public:
  using base::base;
  using base::operator();
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
template<auto f, typename... Args, std::enable_if_t<conjunction<
    std::is_constructible<std::decay_t<Args>, Args>...,
    std::is_move_constructible<std::decay_t<Args>>...,
    not_null<f>
>::value, int> = 0>
constexpr auto bind_front(Args&&... args) {
  return detail::bind_partial_front_const_fn<decltype(f), f, std::decay_t<Args>...>{std::forward<Args>(args)...};
}
#else
template<typename F, F f, typename... Args, std::enable_if_t<conjunction<
    std::is_constructible<std::decay_t<Args>, Args>...,
    std::is_move_constructible<std::decay_t<Args>>...,
    not_null_cxx14<F, f>
>::value, int> = 0>
constexpr auto bind_front(Args&&... args) {
  return detail::bind_partial_front_const_fn<F, f, std::decay_t<Args>...>{std::forward<Args>(args)...};
}
#endif

} // namespace preview

#endif // PREVIEW_FUNCTIONAL_BIND_FRONT_H_
