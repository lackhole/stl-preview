//
// Created by yonggyulee on 2024/01/15.
//

#ifndef PREVIEW_TYPE_TRAITS_IS_NOTHROW_CONVERTIBLE_H_
#define PREVIEW_TYPE_TRAITS_IS_NOTHROW_CONVERTIBLE_H_

#include <type_traits>

#include "preview/__type_traits/bool_constant.h"

namespace preview {
namespace detail {

template<typename From, typename To, bool = std::is_convertible<From, To>::value /* true */>
struct is_nothrow_convertible_impl : std::true_type {};

template<typename From, typename To>
struct is_nothrow_convertible_impl<From, To, false> :
#if __cplusplus < 201703L
    std::true_type
#else
    bool_constant<noexcept( std::declval<void(&)(To) noexcept>()(std::declval<From>()) )>
#endif
    {};
} // namespace detail

template<typename From, typename To>
struct is_nothrow_convertible : detail::is_nothrow_convertible_impl<From, To> {};

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_IS_NOTHROW_CONVERTIBLE_H_
