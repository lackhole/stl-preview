//
// Created by YongGyu Lee on 3/28/24.
//

#ifndef PREVIEW_EXPECTED_MONADIC_H_
#define PREVIEW_EXPECTED_MONADIC_H_

#include <type_traits>
#include <utility>

#include "preview/__expected/unexpect.h"
#include "preview/__expected/unexpected.h"
#include "preview/__functional/invoke.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/is_specialization.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__utility/in_place.h"

namespace preview {
namespace detail {

template<bool Void /* true */, typename T, template<typename...> class Test, typename... U>
struct void_or_impl : std::true_type {};

template<typename T, template<typename...> class Test, typename... U>
struct void_or_impl<false, T, Test, U...> : Test<U...> {};

template<typename T, template<typename...> class Test, typename... U>
using void_or = void_or_impl<std::is_void<T>::value, T, Test, U...>;

template<typename T, typename = void>
struct has_typename_error_type : std::false_type {};

template<typename T>
struct has_typename_error_type<T, void_t<typename T::error_type>> : std::true_type {};

template<typename T>
struct expected_valid_value_type
    : conjunction<
        negation<std::is_reference<T>>,
        negation<std::is_array<T>>,
        negation<std::is_function<T>>,
        negation<is_specialization<T, unexpected>>,
        negation<std::is_same<std::remove_cv_t<T>, in_place_t>>,
        negation<std::is_same<std::remove_cv_t<T>, unexpect_t>>
    > {};

template<typename E>
struct expected_valid_error_type
    : conjunction<
        std::is_object<E>,
        negation<std::is_array<E>>,
        negation<is_specialization<E, unexpected>>,
        negation<std::is_const<E>>,
        negation<std::is_volatile<E>>
    >{};

template<typename E>
struct expected_is_void : std::is_void<typename remove_cvref_t<E>::value_type> {};

template<typename F, typename Expected, bool Void = expected_is_void<Expected>::value /* true */>
struct expected_value_invocable
    : is_invocable<F> {};

template<typename F, typename Expected>
struct expected_value_invocable<F, Expected, false>
    : is_invocable<F, decltype(*std::declval<Expected>())> {};

template<typename F, typename Expected>
struct expected_error_invocable
    : is_invocable<F, decltype(std::declval<Expected>().error())>{};

template<typename F, typename Expected, bool Void = expected_is_void<Expected>::value /* true */>
struct expected_value_invoke_result {
  using type = invoke_result_t<F>;
};

template<typename F, typename Expected>
struct expected_value_invoke_result<F, Expected, false> {
  using type = invoke_result_t<F, decltype(*std::declval<Expected>())>;
};

template<typename F, typename Expected>
using expected_value_invoke_result_t = typename expected_value_invoke_result<F, Expected>::type;

template<typename F, typename Expected>
using expected_error_invoke_result_t = invoke_result_t<F, decltype(std::declval<Expected>().error())>;

template<typename F, typename Expected, std::enable_if_t<
    expected_is_void<Expected>::value == true, int> = 0>
constexpr decltype(auto) expected_value_invoke(F&& f, Expected&&) {
  return preview::invoke(std::forward<F>(f));
}

template<typename F, typename Expected, std::enable_if_t<
    expected_is_void<Expected>::value == false, int> = 0>
constexpr decltype(auto) expected_value_invoke(F&& f, Expected&& e) {
  return preview::invoke(std::forward<F>(f), *std::forward<Expected>(e));
}

template<typename Expected>
class expected_and_then_t;

template<template<typename, typename> class Expected, typename T, typename E>
class expected_and_then_t<Expected<T, E>> {
  template<typename F, typename Self>
  using U = remove_cvref_t<expected_value_invoke_result_t<F, Self>>;

 public:
  template<typename F, typename Self, std::enable_if_t<conjunction<
      expected_value_invocable<F, Self>,
      is_specialization<U<F, Self>, Expected>,
      has_typename_error_type<U<F, Self>>,
      std::is_same<typename U<F, Self>::error_type, E>
  >::value, int> = 0>
  constexpr auto operator()(F&& f, Self&& self) const {
    if (self.has_value()) {
      return expected_value_invoke(std::forward<F>(f), std::forward<Self>(self));
    } else {
      return U<F, Self>(unexpect, std::forward<Self>(self).error());
    }
  }
};

template<typename Expected>
class expected_transform_t;

template<template<typename, typename> class Expected, typename T, typename E>
class expected_transform_t<Expected<T, E>> {
  template<typename F, typename Self>
  using U = std::remove_cv_t<expected_value_invoke_result_t<F, Self>>;

  template<typename F, typename Self>
  constexpr auto valued(F&& f, Self&& self, std::true_type /* is_void<U> */) const {
    expected_value_invoke(std::forward<F>(f), std::forward<Self>(self));
    return Expected<U<F, Self>, E>();
  }

  template<typename F, typename Self>
  constexpr auto valued(F&& f, Self&& self, std::false_type /* is_void<U> */) const {
    return Expected<U<F, Self>, E>(expected_value_invoke(std::forward<F>(f), std::forward<Self>(self)));
  }

 public:
  template<typename F, typename Self, std::enable_if_t<conjunction<
      expected_value_invocable<F, Self>,
      expected_valid_value_type<U<F, Self>>,
      void_or<U<F, Self>, std::is_constructible, U<F, Self>, expected_value_invoke_result_t<F, Self>>
  >::value, int> = 0>
  constexpr auto operator()(F&& f, Self&& self) const {
    if (self.has_value()) {
      return valued(std::forward<F>(f), std::forward<Self>(self), std::is_void<U<F, Self>>{});
    } else {
      return Expected<U<F, Self>, E>(unexpect, std::forward<Self>(self).error());
    }
  }
};

template<typename Expected>
class expected_or_else_t;

template<template<typename, typename> class Expected, typename T, typename E>
class expected_or_else_t<Expected<T, E>> {
  template<typename F, typename Self>
  using G = remove_cvref_t<expected_error_invoke_result_t<F, Self>>;

  template<typename F, typename Self>
  constexpr auto valued(F&&, Self&&, std::true_type /* void */) const {
    return G<F, Self>();
  }

  template<typename F, typename Self>
  constexpr auto valued(F&&, Self&& self, std::false_type /* void */) const {
    return G<F, Self>(in_place, *std::forward<Self>(self));
  }

 public:
  template<typename F, typename Self, std::enable_if_t<conjunction<
      expected_error_invocable<F, Self>,
      is_specialization<G<F, Self>, Expected>,
      std::is_same<typename G<F, Self>::value_type, T>
  >::value, int> = 0>
  constexpr auto operator()(F&& f, Self&& self) const {
    if (self.has_value()) {
      return valued(std::forward<F>(f), std::forward<Self>(self), std::is_void<T>{});
    } else {
      return preview::invoke(std::forward<F>(f), std::forward<Self>(self).error());
    }
  }
};

template<typename Expected>
class expected_transform_error_t;

template<template<typename, typename> class Expected, typename T, typename E>
class expected_transform_error_t<Expected<T, E>> {
  template<typename F, typename Self>
  using G = std::remove_cv_t<expected_error_invoke_result_t<F, Self>>;

  template<typename F, typename Self>
  constexpr auto valued(F&&, Self&&, std::true_type) const {
    return Expected<T, G<F, Self>>();
  }

  template<typename F, typename Self>
  constexpr auto valued(F&&, Self&& e, std::false_type) const {
    return Expected<T, G<F, Self>>(in_place, *std::forward<Self>(e));
  }

 public:
  template<typename F, typename Self, std::enable_if_t<conjunction<
      expected_error_invocable<F, Self>,
      expected_valid_error_type<G<F, Self>>,
      std::is_constructible<G<F, Self>, expected_error_invoke_result_t<F, Self>>
  >::value, int> = 0>
  constexpr auto operator()(F&& f, Self&& self) const {
    if (self.has_value()) {
      return valued(std::forward<F>(f), std::forward<Self>(self), std::is_void<T>{});
    } else {
      return Expected<T, G<F, Self>>(unexpect, preview::invoke(std::forward<F>(f), std::forward<Self>(self).error()));
    }
  }
};

} // namespace detail
} // namespace preview

#endif // PREVIEW_EXPECTED_MONADIC_H_
