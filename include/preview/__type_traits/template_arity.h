//
// Created by yonggyulee on 1/12/24.
//

#ifndef PREVIEW_TYPE_TRAITS_TEMPLATE_ARITY_H_
#define PREVIEW_TYPE_TRAITS_TEMPLATE_ARITY_H_

#include <cstddef>
#include <type_traits>

#include "preview/__type_traits/void_t.h"
#include "preview/__utility/type_sequence.h"

namespace preview {
namespace detail {

template<template<typename...> class C, typename TS, typename = void>
struct can_put_template : std::false_type {};

template<template<typename...> class C, typename... Args>
struct can_put_template<C, type_sequence<Args...>, void_t<C<Args...>>> : std::true_type {};

template<template<typename...> class C, std::size_t N, bool = can_put_template<C, make_type_sequence<int, N>>::value>
struct default_template_arity_impl;

template<template<typename...> class C, std::size_t N>
struct default_template_arity_impl<C, N, true> : std::integral_constant<std::size_t, N> {};

template<template<typename...> class C, std::size_t N>
struct default_template_arity_impl<C, N, false> : default_template_arity_impl<C, N + 1> {};

template<
    template<typename...> class C,
    std::size_t Size,
    bool = can_put_template<C, make_type_sequence<int, Size>>::value>
struct template_arity_impl;

template<template<typename...> class C, std::size_t Size>
struct template_arity_impl<C, Size, true> : std::integral_constant<std::size_t, Size> {};

template<template<typename...> class C, std::size_t Size>
struct template_arity_impl<C, Size, false> : template_arity_impl<C, Size - 1> {};

template<template<typename...> class C>
struct template_arity_impl<C, 0, false> : std::integral_constant<std::size_t, static_cast<std::size_t>(-1)> {};

} // namespace detail


// get template parameter count of template class
template<template<typename...> class C, std::size_t MaxSize = 6>
struct template_arity : detail::template_arity_impl<C, MaxSize>{};


// get mandatory template parameter count of template class
template<template<typename...> class C>
struct mandatory_template_arity : detail::default_template_arity_impl<C, 0> {};


// get default template parameter count of template class
template<template<typename...> class C, std::size_t MaxSize = 6>
struct default_template_arity
    : std::integral_constant<std::size_t, template_arity<C, MaxSize>::value - mandatory_template_arity<C>::value> {};

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_TEMPLATE_ARITY_H_
