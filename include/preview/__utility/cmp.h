//
// Created by YongGyu Lee on 5/8/24.
//

#ifndef PREVIEW_UTILITY_CMP_H_
#define PREVIEW_UTILITY_CMP_H_

#include <type_traits>

#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_character.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace detail {

template<typename T, typename U, typename... SameSignity>
constexpr bool cmp_equal_impl(T t, U u, SameSignity...) noexcept {
  return t == u;
}

template<typename T, typename U>
constexpr bool cmp_equal_impl(T t, U u, std::true_type, std::false_type) noexcept {
  return t >= 0 && std::make_unsigned_t<T>(t) == u;
}

template<typename T, typename U>
constexpr bool cmp_equal_impl(T t, U u, std::false_type, std::true_type) noexcept {
  return u >= 0 && std::make_unsigned_t<U>(u) == t;
}

template<typename T, typename U, typename... SameSignity>
constexpr bool cmp_less_impl(T t, U u, SameSignity...) noexcept {
  return t < u;
}

template<typename T, typename U>
constexpr bool cmp_less_impl(T t, U u, std::true_type, std::false_type) noexcept {
  return t < 0 || std::make_unsigned_t<T>(t) < u;
}

template<typename T, typename U>
constexpr bool cmp_less_impl(T t, U u, std::false_type, std::true_type) noexcept {
  return u >= 0 && t < std::make_unsigned_t<U>(u);
}

template<typename T>
using cmp_requires_one =
    conjunction<
        std::is_integral<T>,
        negation<is_character<T>>,
        negation<std::is_same<T, bool>>
    >;
template<typename T, typename U>
using cmp_requires = conjunction<cmp_requires_one<remove_cvref_t<T>>, cmp_requires_one<remove_cvref_t<U>>>;

} // namespace detail

template<typename T, typename U>
constexpr bool cmp_equal(T t, U u) noexcept {
  static_assert(detail::cmp_requires<T, U>::value, "Both values must be integral, not a character type and not bool");
  return preview::detail::cmp_equal_impl(t, u, std::is_signed<T>{}, std::is_signed<U>{});
}

template<typename T, typename U>
constexpr bool cmp_not_equal(T t, U u) noexcept {
  return !preview::cmp_equal(t, u);
}

template<typename T, typename U>
constexpr bool cmp_less(T t, U u) noexcept {
  static_assert(detail::cmp_requires<T, U>::value, "Both values must be integral, not a character type and not bool");
  return preview::detail::cmp_less_impl(t, u, std::is_signed<T>{}, std::is_signed<U>{});
}

template<typename T, typename U>
constexpr bool cmp_greater(T t, U u) noexcept {
  return preview::cmp_less(u, t);
}

template<class T, class U>
constexpr bool cmp_less_equal(T t, U u) noexcept {
  return !preview::cmp_less(u, t);
}

template<class T, class U>
constexpr bool cmp_greater_equal(T t, U u) noexcept {
  return !preview::cmp_less(t, u);
}

} // namespace preview

#endif // PREVIEW_UTILITY_CMP_H_
