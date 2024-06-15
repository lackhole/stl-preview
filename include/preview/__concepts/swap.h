//
// Created by yonggyulee on 2023/12/21.
//

#ifndef PREVIEW_CONCEPTS_SWAP_H_
#define PREVIEW_CONCEPTS_SWAP_H_

#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__concepts/assignable_from.h"
#include "preview/__concepts/move_constructible.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/is_class_or_enum.h"

namespace preview {
namespace ranges {
namespace detail_ranges_swap {

using std::swap;

template<typename T>
constexpr void swap(T&, T&) = delete;

template<typename T, typename U>
constexpr auto test_swap(int) -> decltype(swap(std::declval<T>(), std::declval<U>()), std::true_type{});

template<typename T, typename U>
constexpr auto test_swap(...) -> std::false_type;

template<typename T, typename U, bool = disjunction<
    is_class_or_enum<remove_cvref_t<T>>,
    is_class_or_enum<remove_cvref_t<U>>>::value>
struct adl_swappable : std::false_type {};

template<typename T, typename U>
struct adl_swappable<T, U, true> : decltype(test_swap<T, U>(0)) {};

template<typename T, typename U, typename = void>
struct swappable_array : std::false_type {};

struct swap_niebloid {
  template<typename T, typename U, std::enable_if_t<
      adl_swappable<T, U>
  ::value, int> = 0>
  constexpr void operator()(T&& t, U&& u) const noexcept(noexcept(swap(std::forward<T>(t), std::forward<U>(u)))) {
    swap(std::forward<T>(t), std::forward<U>(u));
  }

  template<typename T, typename U, std::size_t N, std::enable_if_t<conjunction<
      negation< adl_swappable<T(&)[N], U(&)[N]> >,
      swappable_array<T(&)[N], U(&)[N]>
  >::value, int> = 0>
  constexpr void operator()(T(&t)[N], U(&u)[N]) const noexcept(noexcept((*this)(*t, *u))) {
    for (std::size_t i = 0; i < N; ++i) {
      (*this)(t[i], u[i]);
    }
  }

  template<typename V, std::enable_if_t<conjunction<
      negation< adl_swappable<V&, V&> >,
      move_constructible<V>,
      assignable_from<V&, V>
  >::value, int> = 0>
  constexpr void operator()(V& t, V& u) const
      noexcept(std::is_nothrow_move_constructible<V>::value && std::is_nothrow_move_assignable<V>::value)
  {
    V temp(std::move(t));
    t = std::move(u);
    u = std::move(temp);
  }
};

} // namespace detail_ranges_swap

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail_ranges_swap::swap_niebloid swap{};

} // namespace niebloid
using namespace niebloid;

namespace detail_ranges_swap {

template<typename T, typename U, std::size_t N>
struct swappable_array<T(&)[N], U(&)[N], void_t<decltype(ranges::swap(*std::declval<T(&)[N]>(), *std::declval<U(&)[N]>()))>>
    : std::true_type {};

} // namespace detail_ranges_swap

} // namespace ranges
} // namespace preview

#endif // PREVIEW_CONCEPTS_SWAP_H_
