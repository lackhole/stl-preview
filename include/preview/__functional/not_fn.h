//
// Created by YongGyu Lee on 2024. 7. 9.
//

#ifndef PREVIEW_FUNCTIONAL_NOT_FN_H_
#define PREVIEW_FUNCTIONAL_NOT_FN_H_

#include <type_traits>

#include "preview/__core/std_version.h"
#include "preview/__concepts/different_from.h"
#include "preview/__concepts/not_null.h"
#include "preview/__functional/invoke.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__utility/compressed_pair.h"

namespace preview {
namespace detail {

struct not_fn_object_tag {};

template<typename FD>
class not_fn_object : private compressed_slot<FD, 0> {
  using slot_base = compressed_slot<FD, 0>;

 public:
  template<typename F>
  constexpr explicit not_fn_object(F&& f, not_fn_object_tag)
      noexcept(std::is_nothrow_constructible<FD, F>::value)
      : slot_base(std::forward<F>(f)) {}

  not_fn_object(not_fn_object const &) = default;
  not_fn_object(not_fn_object &&) = default;

  template<typename... Args>
  constexpr auto operator()(Args&&... args) & noexcept(
         noexcept(!preview::invoke(std::declval<FD&>()         , std::forward<Args>(args)...)))
      -> decltype(!preview::invoke(std::declval<FD&>()         , std::forward<Args>(args)...))
         { return !preview::invoke(slot_base::template get<0>(), std::forward<Args>(args)...); }

  template<typename... Args>
  constexpr auto operator()(Args&&... args) const & noexcept(
         noexcept(!preview::invoke(std::declval<const FD&>()   , std::forward<Args>(args)...)))
      -> decltype(!preview::invoke(std::declval<const FD&>()   , std::forward<Args>(args)...))
         { return !preview::invoke(slot_base::template get<0>(), std::forward<Args>(args)...); }

  template<typename... Args>
  constexpr auto operator()(Args&&... args) && noexcept(
         noexcept(!preview::invoke(std::declval<FD&&>()        , std::forward<Args>(args)...)))
      -> decltype(!preview::invoke(std::declval<FD&&>()        , std::forward<Args>(args)...))
         { return !preview::invoke(slot_base::template get<0>(), std::forward<Args>(args)...); }

  template<typename... Args>
  constexpr auto operator()(Args&&... args) const && noexcept(
         noexcept(!preview::invoke(std::declval<const FD&&>()  , std::forward<Args>(args)...)))
      -> decltype(!preview::invoke(std::declval<const FD&&>()  , std::forward<Args>(args)...))
         { return !preview::invoke(slot_base::template get<0>(), std::forward<Args>(args)...); }
};

template<typename F, F ConstFn>
class not_fn_object_stateless {
 public:
  template<typename... Args>
  constexpr auto operator()(Args&&... args) const noexcept(
         noexcept(!preview::invoke(ConstFn, std::forward<Args>(args)...)))
      -> decltype(!preview::invoke(ConstFn, std::forward<Args>(args)...))
         { return !preview::invoke(ConstFn, std::forward<Args>(args)...); }
};

} // namespace detail

template<typename F>
constexpr auto not_fn(F&& f) {
  return detail::not_fn_object<std::decay_t<F>>{std::forward<F>(f), detail::not_fn_object_tag{}};
}

#if PREVIEW_CXX_VERSION >= 17
template<auto F>
constexpr std::enable_if_t<
    not_null<F>::value,
    detail::not_fn_object_stateless<decltype(F), F>
> not_fn() noexcept { return {}; }
#else
template<typename F, F ConstFn>
constexpr std::enable_if_t<
    not_null_cxx14<F, ConstFn>::value,
    detail::not_fn_object_stateless<F, ConstFn>
> not_fn() noexcept { return {}; }
#endif

} // namespace preview

#endif // PREVIEW_FUNCTIONAL_NOT_FN_H_
