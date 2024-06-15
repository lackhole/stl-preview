//
// Created by yonggyulee on 2023/10/27.
//

#ifndef PREVIEW_TYPE_TRAITS_HAS_TYPENAME_TYPE_H_
#define PREVIEW_TYPE_TRAITS_HAS_TYPENAME_TYPE_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__type_traits/void_t.h"

namespace preview {

template<typename T, typename = void>
struct has_typename_type : std::false_type {};

template<typename T>
struct has_typename_type<T, void_t<typename T::type>> : std::true_type {};

template<typename... B>
PREVIEW_INLINE_VARIABLE constexpr bool has_typename_type_v = has_typename_type<B...>::value;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_HAS_TYPENAME_TYPE_H_
