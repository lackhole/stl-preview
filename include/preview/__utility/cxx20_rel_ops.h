//
// Created by yonggyulee on 2023/12/26.
//

#ifndef PREVIEW_UTILITY_CXX20_REL_OPS_H_
#define PREVIEW_UTILITY_CXX20_REL_OPS_H_

#include <type_traits>

#include "preview/__core/std_version.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/is_referenceable.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/void_t.h"

// automatically generates comparison operators based on user-defined `operator==` and `operator<` in C++20 way

namespace preview {
namespace rel_ops {
namespace detail {

template<typename T, typename U, typename = void>
struct has_operator_equal_2 : std::false_type {};
template<typename T, typename U>
struct has_operator_equal_2<
        T, U,
        void_t<decltype( std::declval<T>() == std::declval<U>() )>
    > : std::is_convertible<decltype( std::declval<T>() == std::declval<U>() ), bool> {};

template<typename T, typename U, typename = void>
struct has_operator_less_2 : std::false_type {};
template<typename T, typename U>
struct has_operator_less_2<
        T, U,
        void_t<decltype( std::declval<T>() < std::declval<U>() )>
    > : std::is_convertible<decltype( std::declval<T>() < std::declval<U>() ), bool> {};

} // namespace detail

#if __cplusplus < 202002L

// synthesized from `U == T`
template<typename T, typename U, std::enable_if_t<conjunction<
    negation<std::is_same<T, U>>,
    detail::has_operator_equal_2<const U&, const T&>
>::value, int> = 0>
constexpr bool operator==(const T& a, const U& b) noexcept(noexcept(b == a)) {
  return b == a;
}

#endif // __cplusplus < 202002L

namespace detail {

struct is_equality_comparable_impl {
  template<typename T, typename U>
  static constexpr std::false_type test(...);
  template<typename T, typename U>
  static constexpr auto test(void_t<decltype( std::declval<T>() == std::declval<U>() )>* = nullptr)
      -> std::is_convertible<decltype( std::declval<T>() == std::declval<U>() ), bool>;

  template<typename T, typename U>
  using type = decltype(test<T, U>(0));
};

} // namespace detail

template<typename T, typename U>
struct is_equality_comparable : detail::is_equality_comparable_impl::type<T, U> {};


// return `!( b < a || a == b)`. synthesized from `U < T` and `T == U`
template<typename T, typename U, std::enable_if_t<conjunction<
    negation<std::is_same<T, U>>,
    is_equality_comparable<T, U>,
    negation< detail::has_operator_less_2<const T&, const U&> >,
    detail::has_operator_less_2<const U&, const T&>
  >::value, int> = 0>
constexpr bool operator<(const T& a, const U& b) noexcept(noexcept(!( (b < a) || (a == b)))) {
  // (a < b) -> !(a >= b) -> !( a > b || a == b) -> !( b < a || a == b)
  return !( (b < a) || (a == b));
}

namespace detail {

struct is_less_than_comparable_impl {
  template<typename T, typename U>
  static constexpr std::false_type test(...);
  template<typename T, typename U>
  static constexpr auto test(void_t<decltype( std::declval<T>() < std::declval<U>() )>* = nullptr)
      -> std::is_convertible<decltype( std::declval<T>() < std::declval<U>() ), bool>;

  template<typename T, typename U>
  using type = decltype(test<T, U>(0));
};

} // namespace detail

template<typename T, typename U>
struct is_less_than_comparable : detail::is_less_than_comparable_impl::type<T, U> {};

// return `!(a == b)`. Synthesized from `T == U`
template<typename T, typename U, std::enable_if_t<is_equality_comparable<T, U>::value, int> = 0>
constexpr bool operator!=(const T& a, const U& b) noexcept(noexcept(!(a == b))) {
  return !(a == b);
}

// return `b < a`. Synthesized from `U < T`
template<typename T, typename U, std::enable_if_t<is_less_than_comparable<U, T>::value, int> = 0>
constexpr bool operator>(const T& a, const U& b) noexcept(noexcept(b < a)) {
  return b < a; // (a > b) -> (b < a)
}

// return `!(b < a)`. Synthesized from `U < T`
template<typename T, typename U, std::enable_if_t<is_less_than_comparable<U, T>::value, int> = 0>
constexpr bool operator<=(const T& a, const U& b) noexcept(noexcept(!(b < a))) {
  return !(b < a); // (a <= b) -> !(a > b) -> !(b < a)
}

// return `!(a < b)`. Synthesized from `T < U`
template<typename T, typename U, std::enable_if_t<is_less_than_comparable<T, U>::value, int> = 0>
constexpr bool operator>=(const T& a, const U& b) noexcept(noexcept(!(a < b))) {
  return !(a < b); // (a >= b) -> !(a < b)
}

namespace detail {

struct is_non_equality_comparable_impl {
  template<typename T, typename U>
  static constexpr std::false_type test(...);
  template<typename T, typename U>
  static constexpr auto test(void_t<decltype( std::declval<T>() != std::declval<U>() )>* = nullptr)
      -> std::is_convertible<decltype( std::declval<T>() != std::declval<U>() ), bool>;

  template<typename T, typename U>
  using type = decltype(test<T, U>(0));
};

struct is_less_equal_than_comparable_impl {
  template<typename T, typename U>
  static constexpr std::false_type test(...);
  template<typename T, typename U>
  static constexpr auto test(void_t<decltype( std::declval<T>() <= std::declval<U>() )>* = nullptr)
      -> std::is_convertible<decltype( std::declval<T>() <= std::declval<U>() ), bool>;

  template<typename T, typename U>
  using type = decltype(test<T, U>(0));
};

struct is_greater_than_comparable_impl {
  template<typename T, typename U>
  static constexpr std::false_type test(...);
  template<typename T, typename U>
  static constexpr auto test(void_t<decltype( std::declval<T>() > std::declval<U>() )>* = nullptr)
      -> std::is_convertible<decltype( std::declval<T>() > std::declval<U>() ), bool>;

  template<typename T, typename U>
  using type = decltype(test<T, U>(0));
};

struct is_greater_equal_than_comparable_impl {
  template<typename T, typename U>
  static constexpr std::false_type test(...);
  template<typename T, typename U>
  static constexpr auto test(void_t<decltype( std::declval<T>() >= std::declval<U>() )>* = nullptr)
      -> std::is_convertible<decltype( std::declval<T>() >= std::declval<U>() ), bool>;

  template<typename T, typename U>
  using type = decltype(test<T, U>(0));
};

} // namespace detail

template<typename T, typename U>
struct is_non_equality_comparable : detail::is_non_equality_comparable_impl::type<T, U> {};
template<typename T, typename U>
struct is_less_equal_than_comparable : detail::is_less_equal_than_comparable_impl::type<T, U> {};
template<typename T, typename U>
struct is_greater_than_comparable : detail::is_greater_than_comparable_impl::type<T, U> {};
template<typename T, typename U>
struct is_greater_equal_than_comparable : detail::is_greater_equal_than_comparable_impl::type<T, U> {};

} // namespace rel_ops
} // namespace preview

#endif // PREVIEW_UTILITY_CXX20_REL_OPS_H_
