//
// Created by yonggyulee on 2024. 7. 13..
//

#ifndef PREVIEW_TYPE_TRAITS_IS_DEDUCTIBLE_H_
#define PREVIEW_TYPE_TRAITS_IS_DEDUCTIBLE_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__core/cxx_version.h"


namespace preview {
namespace detail {

template<template<typename...> class Class, typename TArgs, typename = void>
struct is_deductible_impl : std::false_type {};

#if PREVIEW_CXX_VERSION >= 17
template<template<typename...> class Class, typename... T>
struct is_deductible_impl<Class, std::tuple<T...>, void_t<decltype(Class(std::declval<T>()...))>> : std::true_type {};
#endif

} // namespace detail

template<template<typename...> class Class, typename... T>
struct is_deductible : detail::is_deductible_impl<Class, std::tuple<T...>> {};

template<template<typename...> class Class, typename... T>
constexpr PREVIEW_INLINE_VARIABLE bool is_deductible_v = is_deductible<Class, T...>::value;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_IS_DEDUCTIBLE_H_
