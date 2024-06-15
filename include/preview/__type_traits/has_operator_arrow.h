//
// Created by yonggyulee on 1/24/24.
//

#ifndef PREVIEW_TYPE_TRAITS_HAS_OPERATOR_ARROW_H_
#define PREVIEW_TYPE_TRAITS_HAS_OPERATOR_ARROW_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__type_traits/void_t.h"

namespace preview {

template<typename T, typename = void>
struct has_operator_arrow : std::false_type {};

template<typename T>
struct has_operator_arrow<T, void_t<decltype(std::declval<T>().operator->())>> : std::true_type {};

template<typename T>
PREVIEW_INLINE_VARIABLE constexpr bool has_operator_arrow_v = has_operator_arrow<T>::value;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_HAS_OPERATOR_ARROW_H_
