//
// Created by YongGyu Lee on 2024. 7. 6.
//

#ifndef PREVIEW_CONCEPTS_NOT_NULL_H_
#define PREVIEW_CONCEPTS_NOT_NULL_H_

#include "preview/__core/std_version.h"

#include <type_traits>

#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/disjunction.h"

namespace preview {
namespace detail {

template<typename T, T x, bool =
    disjunction<
        std::is_pointer<T>,
        std::is_member_pointer<T>,
        std::is_null_pointer<T>
    >::value /* false */>
struct not_null_impl : std::true_type {};

template<typename T, T x>
struct not_null_impl<T, x, true> : bool_constant<(x != nullptr)> {};

} // namespace detail

#if PREVIEW_CXX_VERSION >= 17
template<auto x>
struct not_null : detail::not_null_impl<decltype(x), x> {};
#endif

template<typename T, T x>
struct not_null_cxx14 : detail::not_null_impl<T, x> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_NOT_NULL_H_
