#ifndef PREVIEW_FUNCTIONAL_BIND_BACK_H_
#define PREVIEW_FUNCTIONAL_BIND_BACK_H_

#include <cstddef>
#include <type_traits>
#include <utility>

#include "preview/__functional/invoke.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__utility/compressed_pair.h"

namespace preview {
namespace detail {

template<typename FD, typename... BoundArgs>
class bind_back_object {
  using indices = std::index_sequence_for<BoundArgs...>;

 public:
  template<typename F, typename... Args, std::enable_if_t<
      negation<std::is_same<bind_back_object, remove_cvref_t<F>>>
  ::value, int> = 0>
  constexpr explicit bind_back_object(F&& f, Args&&... args)
      : pair_(compressed_pair_variadic_construct_divider<1>{},
              std::forward<F>(f),
              std::forward<Args>(args)...) {}

  template<typename... CallArgs>
  constexpr invoke_result_t<FD&, CallArgs&&..., BoundArgs&...> operator()(CallArgs&&... call_args) &
      noexcept(is_nothrow_invocable<FD&, CallArgs&&..., BoundArgs&...>::value)
  {
    return call(pair_.first(), pair_.second(), indices{}, std::forward<CallArgs>(call_args)...);
  }

  template<typename... CallArgs>
  constexpr invoke_result_t<const FD&, CallArgs&&..., const BoundArgs&...> operator()(CallArgs&&... call_args) const &
      noexcept(is_nothrow_invocable<const FD&, CallArgs&&..., const BoundArgs&...>::value)
  {
    return call(pair_.first(), pair_.second(), indices{}, std::forward<CallArgs>(call_args)...);
  }

  template<typename... CallArgs>
  constexpr invoke_result_t<FD&&, CallArgs&&..., BoundArgs&&...> operator()(CallArgs&&... call_args) &&
      noexcept(is_nothrow_invocable<FD&&, CallArgs&&..., BoundArgs&&...>::value)
  {
    return call(std::move(pair_.first()), std::move(pair_.second()), indices{}, std::forward<CallArgs>(call_args)...);
  }

  template<typename... CallArgs>
  constexpr invoke_result_t<const FD&&, CallArgs&&..., const BoundArgs&&...> operator()(CallArgs&&... call_args) const &&
      noexcept(is_nothrow_invocable<const FD&&, CallArgs&&..., const BoundArgs&&...>::value)
  {
    return call(std::move(pair_.first()), std::move(pair_.second()), indices{}, std::forward<CallArgs>(call_args)...);
  }

 private:
  template<typename F, typename BoundArgsTuple, std::size_t...I, typename... Args>
  static constexpr auto call(F&& f, BoundArgsTuple&& tup, std::index_sequence<I...>, Args&&... args)
      noexcept(noexcept(
          preview::invoke(std::forward<F>(f), std::forward<Args>(args)..., std::get<I>(std::forward<BoundArgsTuple>(tup))...)
      ))
  {
    return preview::invoke(std::forward<F>(f), std::forward<Args>(args)..., std::get<I>(std::forward<BoundArgsTuple>(tup))...);
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
constexpr auto bind_back(F&& f, Args&&... args) {
  return detail::bind_back_object<std::decay_t<F>, std::decay_t<Args>...>{
      std::forward<F>(f),
      std::forward<Args>(args)...
  };
}

} // namespace preview

#endif // PREVIEW_FUNCTIONAL_BIND_BACK_H_
