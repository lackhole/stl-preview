//
// Created by yonggyulee on 2024/02/08.
//

#ifndef PREVIEW_TYPE_TRAITS_IS_CLASS_OR_ENUM_H_
#define PREVIEW_TYPE_TRAITS_IS_CLASS_OR_ENUM_H_

#include <type_traits>

#include "preview/core.h"
#include "preview/__type_traits/disjunction.h"

namespace preview {

template<class T>
struct is_class_or_enum : disjunction<std::is_class<T>, std::is_enum<T>> {};

template<typename T>
PREVIEW_INLINE_VARIABLE constexpr bool is_class_or_enum_v = is_class_or_enum<T>::value;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_IS_CLASS_OR_ENUM_H_
