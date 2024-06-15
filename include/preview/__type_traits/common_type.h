//
// Created by yonggyulee on 2023/10/27.
//

#ifndef PREVIEW_TYPE_TRAITS_COMMON_TYPE_H_
#define PREVIEW_TYPE_TRAITS_COMMON_TYPE_H_

#include <chrono>
#include <type_traits>
#include <utility>

#include "preview/__tuple/tuple_like.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_referenceable.h"
#include "preview/__type_traits/void_t.h"

namespace preview {

template<typename ...T>
struct common_type;

namespace detail {

struct no_common_type {};

template<typename T, typename U>
using common_type_ternary = std::decay_t<decltype(false ? std::declval<T>() : std::declval<U>())>;

template<typename CR1, typename CR2>
struct common_type_test_3_1 {
  using type = common_type_ternary<CR1, CR2>;
};

// C++20 check
template<typename CR1, typename CR2, typename = void>
struct common_type_test_3 : no_common_type {};

template<typename CR1, typename CR2>
struct common_type_test_3<CR1, CR2, void_t<common_type_ternary<CR1, CR2>>>
    : common_type_test_3_1<CR1, CR2> {};

// Check if referencable
template<typename T1, typename T2,
    bool v1 = is_referencable<std::remove_reference_t<T1>>::value,
    bool v2 = is_referencable<std::remove_reference_t<T2>>::value>
struct common_type_test_2 : no_common_type {};

template<typename T1, typename T2>
struct common_type_test_2<T1, T2, true, true>
    : common_type_test_3<const std::remove_reference_t<T1>&, const std::remove_reference_t<T2>&> {};

template<typename T1, typename T2>
struct common_type_test_1_1 {
  using type = common_type_ternary<T1, T2>;
};

template<typename T1, typename T2, typename = void>
struct common_type_test_1 : common_type_test_2<T1, T2> {};

template<typename T1, typename T2>
struct common_type_test_1<T1, T2, void_t<common_type_ternary<T1, T2>>>
    : common_type_test_1_1<T1, T2> {};


template<typename T1, typename T2, bool /* false */>
struct common_type_tuple_like_3 : common_type_test_1<T1, T2> {};

template<template<typename...> class Tuple, typename... T, typename... U>
struct common_type_tuple_like_3<Tuple<T...>, Tuple<U...>, true> {
  using type = Tuple<typename common_type<T, U>::type...>;
};

template<
    typename T1,
    typename T2,
    bool = bool_constant<(std::tuple_size<T1>::value == std::tuple_size<T2>::value)>::value /* false */
>
struct common_type_tuple_like_2 : common_type_test_1<T1, T2> {};

template<template<typename...> class TTuple, template<typename...> class UTuple, typename... T, typename... U>
struct common_type_tuple_like_2<TTuple<T...>, UTuple<U...>, true>
    : common_type_tuple_like_3<
          TTuple<T...>,
          UTuple<U...>,
          conjunction<
              has_typename_type<common_type<T, U>>...
          >::value
      > {};

template<
    typename T1,
    typename T2,
    bool = conjunction<
        tuple_like<T1>,
        tuple_like<T2>,
        std::is_same<T1, std::decay_t<T1>>,
        std::is_same<T2, std::decay_t<T2>>
    >::value /* true */
>
struct common_type_tuple_like : common_type_tuple_like_2<T1, T2> {};

template<typename T1, typename T2>
struct common_type_tuple_like<T1, T2, false> : common_type_test_1<T1, T2> {};


template<bool HasType /* false */, typename CommonType, typename... Ts>
struct common_type_test_three_or_more : no_common_type {};

template<typename CommonType, typename... Ts>
struct common_type_test_three_or_more<true, CommonType, Ts...>
    : common_type<typename CommonType::type, Ts...> {};

} // namespace detail

template<>
struct common_type<> : detail::no_common_type {};

template<typename T>
struct common_type<T> : common_type<T, T> {};

template<typename T1, typename T2>
struct common_type<T1, T2> : detail::common_type_tuple_like<std::decay_t<T1>, std::decay_t<T2>> {};

template<typename T1, typename T2, typename ...Ts>
struct common_type<T1, T2, Ts...>
    : detail::common_type_test_three_or_more<
          has_typename_type<common_type<T1, T2>>::value,
          common_type<T1, T2>,
          Ts...> {};

template<typename ...T>
using common_type_t = typename common_type<T...>::type;


// Specialization for std
namespace detail {

template<typename T, typename U, bool /* false */>
struct common_type_specialization_impl : no_common_type {};

template<template<typename...> class C, typename... T, typename... U>
struct common_type_specialization_impl<C<T...>, C<U...>, true> {
  using type = C<common_type_t<T, U>...>;
};

template<typename T, typename U>
struct common_type_specialization;

template<template<typename...> class C, typename... T, typename... U>
struct common_type_specialization<C<T...>, C<U...>>
    : common_type_specialization_impl<
          C<T...>,
          C<U...>,
          conjunction<
              has_typename_type<common_type<T, U>>...
          >::value
      > {};

} // namespace detail

template<typename T1, typename T2, typename U1, typename U2>
struct common_type<std::pair<T1, T2>, std::pair<U1, U2>>
    : detail::common_type_specialization<std::pair<T1, T2>, std::pair<U1, U2>> {};

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
struct common_type<std::chrono::duration<Rep1, Period1>, std::chrono::duration<Rep2, Period2>>
    : detail::common_type_specialization<std::chrono::duration<Rep1, Period1>, std::chrono::duration<Rep2, Period2>> {};

template<typename Clock, typename Duration1, typename Duration2>
struct common_type<std::chrono::time_point<Clock, Duration1>, std::chrono::time_point<Clock, Duration2>>
    : detail::common_type_specialization<std::chrono::time_point<Clock, Duration1>, std::chrono::time_point<Clock, Duration2>> {};

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_COMMON_TYPE_H_
