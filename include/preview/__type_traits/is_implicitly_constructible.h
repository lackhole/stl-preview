//
// Created by YongGyu Lee on 11/13/23.
//

#ifndef PREVIEW_TYPE_TRAITS_IS_IMPLICITLY_CONSTRUCTIBLE_H_
#define PREVIEW_TYPE_TRAITS_IS_IMPLICITLY_CONSTRUCTIBLE_H_

#include <type_traits>

#include "preview/core.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_implicitly_default_constructible.h"
#include "preview/__type_traits/negation.h"

namespace preview {
namespace detail {

template<typename T, typename... Args>
struct is_implicitly_constructible_impl :
    conjunction<
        std::is_constructible<T, Args...>,
        std::is_convertible<Args..., T>
    > {};

template<typename T>
struct is_implicitly_constructible_impl<T> : is_implicitly_default_constructible<T> {};

} // namespace detail

template<typename T, typename... Args>
struct is_implicitly_constructible : detail::is_implicitly_constructible_impl<T, Args...> {};

template<typename T, typename... Args>
PREVIEW_INLINE_VARIABLE constexpr bool is_implicitly_constructible_v = is_implicitly_constructible<T, Args...>::value;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_IS_IMPLICITLY_CONSTRUCTIBLE_H_
