//
// Created by YongGyu Lee on 11/3/23.
//

#ifndef PREVIEW_TYPE_TRAITS_IS_LIST_INITIALIZABLE_H_
#define PREVIEW_TYPE_TRAITS_IS_LIST_INITIALIZABLE_H_

#include <type_traits>

#include "preview/core.h"
#include "preview/__type_traits/void_t.h"

namespace preview {

namespace detail {

template<typename To, typename From, typename = void>
struct is_list_initializable_impl : std::false_type {};

template<typename To, typename From>
struct is_list_initializable_impl<To, From, void_t<decltype(To{std::declval<From>()})>> : std::true_type {};

} // namespace detail

template<typename To, typename From>
struct is_list_initializable : detail::is_list_initializable_impl<To, From> {};

template<typename To, typename From>
struct is_copy_list_initializable : std::conditional_t<is_list_initializable<To, From>::value, std::is_convertible<From, To>, std::false_type> {};

template<typename To, typename From>
PREVIEW_INLINE_VARIABLE constexpr bool is_list_initializable_v = is_list_initializable<To, From>::value;

template<typename To, typename From>
PREVIEW_INLINE_VARIABLE constexpr bool is_copy_list_initializable_v = is_copy_list_initializable<To, From>::value;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_IS_LIST_INITIALIZABLE_H_
