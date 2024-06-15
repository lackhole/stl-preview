//
// Created by yonggyulee on 1/31/24.
//

#ifndef PREVIEW_VARIANT_VISIT_H_
#define PREVIEW_VARIANT_VISIT_H_

#include <tuple>
#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__functional/invoke.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__variant/variant.h"
#include "preview/__variant/variant_alternative.h"

namespace preview {
namespace detail {

template<std::size_t Cursor, typename R>
struct visitor_global {
  template<typename T, std::size_t J, typename Vis, std::size_t... I, typename... Variants>
  constexpr R operator()(T&&, in_place_index_t<J>, Vis&& vis, std::index_sequence<I...>, Variants&&... vars) const {
    return visitor_global<Cursor - 1, R>{}.visit(
        std::forward<Vis>(vis), std::index_sequence<I..., J>{}, std::forward<Variants>(vars)...);
  }

  template<typename U, typename Vis, std::size_t... I, typename... Variants>
  constexpr R operator()(U&&, in_place_index_t<variant_npos>, Vis&& vis, std::index_sequence<I...>, Variants&&... vars) const {
    throw bad_variant_access{};
    return visitor_global<Cursor - 1, R>{}.visit(
        std::forward<Vis>(vis), std::index_sequence<I..., 0>{}, std::forward<Variants>(vars)...);
  }

  template<typename Vis, std::size_t...I, typename... Variants>
  constexpr R visit(Vis&& vis, std::index_sequence<I...>, Variants&&... vars) const {
    auto&& var = std::get<Cursor - 1>(std::forward_as_tuple(std::forward<Variants>(vars)...));
    return variant_raw_visit(
        var.index(), std::forward<decltype(var._base().storage())>(var._base().storage()), *this,
        std::forward<Vis>(vis), std::index_sequence<I...>{}, std::forward<Variants>(vars)...);
  }
};

template<typename R>
struct visitor_global<0, R> {
  template<typename Vis, std::size_t...I, typename... Variants>
  constexpr R visit(Vis&& vis, std::index_sequence<I...>,  Variants&&... vars) const {
    return preview::invoke_r<R>(
        std::forward<Vis>(vis),
        std::forward<decltype(variant_raw_get(vars._base().storage(), in_place_index<I>))>(
            variant_raw_get(vars._base().storage(), in_place_index<I>))...
    );
  }
};

struct as_variant_niebloid {
  template<typename... Ts>
  constexpr variant<Ts...>& operator()(variant<Ts...>& var) const noexcept { return var; }

  template<typename... Ts>
  constexpr const variant<Ts...>& operator()(const variant<Ts...>& var) const noexcept { return var;}

  template<typename... Ts>
  constexpr variant<Ts...>&& operator()(variant<Ts...>&& var) const noexcept { return std::move(var); }

  template<typename... Ts>
  constexpr const variant<Ts...>&& operator()(const variant<Ts...>&& var) const noexcept { return std::move(var); }
};

PREVIEW_INLINE_VARIABLE constexpr as_variant_niebloid as_variant{};

template<typename Variant>
using as_variant_t = invoke_result_t<as_variant_niebloid, Variant>;

template<typename Visitor, typename... Variants>
struct variant_visit_result {
  using type = invoke_result_t<Visitor, decltype(get<0>(std::declval<Variants>()))...>;
};

template<typename Visitor, typename... Variants>
using variant_visit_result_t = typename variant_visit_result<Visitor, as_variant_t<Variants>...>::type;

} // namespace detail

template<typename R, typename Visitor, typename... Variants, std::enable_if_t<conjunction<
    is_invocable<detail::as_variant_niebloid, Variants>...
>::value, int> = 0>
constexpr R visit(Visitor&& vis, Variants&&... vars) {
  return detail::visitor_global<sizeof...(Variants), R>{}.visit(
      std::forward<Visitor>(vis),
      std::index_sequence<>{},
      detail::as_variant(std::forward<Variants>(vars))...
  );
}

template<typename Visitor, typename... Variants, std::enable_if_t<conjunction<
    is_invocable<detail::as_variant_niebloid, Variants>...
>::value, int> = 0>
constexpr decltype(auto)
visit(Visitor&& vis, Variants&&... vars) {
  using R = detail::variant_visit_result_t<Visitor, detail::as_variant_t<Variants>...>;
  return visit<R>(std::forward<Visitor>(vis), std::forward<Variants>(vars)...);
}

namespace detail {


template<typename Visitor, typename Variant>
constexpr decltype(auto) visit_single(Visitor&& vis, Variant&& var) {
  return preview::visit(std::forward<Visitor>(vis), std::forward<Variant>(var));
}

template<typename R, typename Visitor, typename Variant>
constexpr R visit_single(Visitor&& vis, Variant&& var) {
  return preview::visit<R>(std::forward<Visitor>(vis), std::forward<Variant>(var));
}

} // namespace detail

} // namespace preview

#endif // PREVIEW_VARIANT_VISIT_H_
