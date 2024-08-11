//
// Created by yonggyulee on 2023/10/27.
//

#ifndef PREVIEW_TYPE_TRAITS_COMMON_TYPE_H_
#define PREVIEW_TYPE_TRAITS_COMMON_TYPE_H_

#include <chrono>
#include <type_traits>
#include <utility>

#include "preview/__tuple/tuple_like.h"
#include "preview/__type_traits/detail/test_ternary.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_referenceable.h"
#include "preview/__type_traits/no_traits.h"
#include "preview/__type_traits/void_t.h"

namespace preview {

// Forward declare
// User specialization for `std::common_type` is used if valid
template<typename ...T>
struct common_type;

namespace detail {

template<template<typename...> class Class, typename T1, typename T2>
struct common_type_template_class {};

template<template<typename...> class Class,
         template<typename...> class TClass, typename... T,
         template<typename...> class UClass, typename... U>
struct common_type_template_class<Class, TClass<T...>, UClass<U...>> {
  using type = Class<typename common_type<T, U>::type...>;
};

template<typename T1, typename T2, bool = /* false */ conjunction<
    tuple_like<T1>,
    tuple_like<T2>,
    std::is_same<T1, std::decay_t<T1>>,
    std::is_same<T2, std::decay_t<T2>>
>::value>
struct common_type_satisfies_tuple_like_constraints : std::false_type {};

template<template<typename...> class TTuple, template<typename...> class UTuple, typename... T, typename... U>
struct common_type_satisfies_tuple_like_constraints<TTuple<T...>, UTuple<U...>, true>
    : conjunction<
        bool_constant<(std::tuple_size<TTuple<T...>>::value == std::tuple_size<UTuple<U...>>::value)>,
        has_typename_type<common_type<T, U>>...
    > {};

template<typename T1, typename T2>
struct common_type_std_fallback : std::common_type<T1, T2> {};

template<typename T1, typename T2, bool = /* false */ conjunction<is_referencable<T1>, is_referencable<T2>>::value>
struct common_type_satisfies_cref : std::false_type  {};
template<typename T1, typename T2>
struct common_type_satisfies_cref<T1, T2, true>
    : has_typename_type<test_ternary<const std::remove_reference_t<T1>&, const std::remove_reference_t<T2>&>> {};

template<typename T1, typename T2, bool = /* false */ common_type_satisfies_cref<T1, T2>::value>
struct common_type_primary_cref : common_type_std_fallback<T1, T2> {};
template<typename T1, typename T2>
struct common_type_primary_cref<T1, T2, true> {
  using type = std::decay_t<test_ternary_t<const std::remove_reference_t<T1>&, const std::remove_reference_t<T2>&>>;
};

template<typename T1, typename T2, bool = /* false */ has_typename_type<test_ternary<T1, T2>>::value>
struct common_type_primary : common_type_primary_cref<T1, T2> {};

template<typename T1, typename T2>
struct common_type_primary<T1, T2, true> {
  using type = std::decay_t<test_ternary_t<T1, T2>>;
};

template<typename T1, typename T2>
struct common_type_two
    : std::conditional_t<
        common_type_satisfies_tuple_like_constraints<T1, T2>::value,
        common_type_template_class<std::tuple, T1, T2>,
        common_type_primary<T1, T2>
    > {};

template<bool HasType /* false */, typename CommonType, typename... Ts>
struct common_type_test_three_or_more {};

template<typename CommonType, typename... Ts>
struct common_type_test_three_or_more<true, CommonType, Ts...>
    : common_type<typename CommonType::type, Ts...> {};

} // namespace detail

template<>
struct common_type<> {};

template<typename T>
struct common_type<T> : common_type<T, T> {};

template<typename T1, typename T2>
struct common_type<T1, T2> : detail::common_type_two<std::decay_t<T1>, std::decay_t<T2>> {};

template<typename T1, typename T2, typename ...Ts>
struct common_type<T1, T2, Ts...>
    : detail::common_type_test_three_or_more<
          has_typename_type<common_type<T1, T2>>::value,
          common_type<T1, T2>,
          Ts...
    > {};

template<typename ...T>
using common_type_t = typename common_type<T...>::type;

// Specializations for std
template<typename T1, typename T2, typename U1, typename U2>
struct common_type<std::pair<T1, T2>, std::pair<U1, U2>>
    : detail::common_type_template_class<std::pair, std::pair<T1, T2>, std::pair<U1, U2>> {};

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
struct common_type<std::chrono::duration<Rep1, Period1>, std::chrono::duration<Rep2, Period2>>
    : detail::common_type_template_class<
        std::chrono::duration,
        std::chrono::duration<Rep1, Period1>,
        std::chrono::duration<Rep2, Period2>
    > {};

template<typename Clock, typename Duration1, typename Duration2>
struct common_type<std::chrono::time_point<Clock, Duration1>, std::chrono::time_point<Clock, Duration2>>
    : detail::common_type_template_class<
        std::chrono::time_point,
        std::chrono::time_point<Clock, Duration1>,
        std::chrono::time_point<Clock, Duration2>
    > {};

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_COMMON_TYPE_H_
