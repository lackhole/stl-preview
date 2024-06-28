//
// Created by YongGyu Lee on 2024. 6. 29.
//

#ifndef PREVIEW_TYPE_TRAITS_HAS_CONVERSION_OPERATOR_H_
#define PREVIEW_TYPE_TRAITS_HAS_CONVERSION_OPERATOR_H_

#include <type_traits>

#include "preview/__type_traits/void_t.h"

namespace preview {

template<typename From, typename To, typename = void>
struct has_conversion_operator
    : std::false_type {};

template<typename From, typename To>
struct has_conversion_operator<From, To, void_t<decltype( std::declval<From>().operator To() )>>
    : std::is_same<decltype( std::declval<From>().operator To() ), To> {};

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_HAS_CONVERSION_OPERATOR_H_
