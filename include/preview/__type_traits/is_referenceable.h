//
// Created by yonggyulee on 2023/10/27.
//

#ifndef PREVIEW_TYPE_TRAITS_IS_REFERENCEABLE_H_
#define PREVIEW_TYPE_TRAITS_IS_REFERENCEABLE_H_

#include <type_traits>

#include "preview/core.h"
#include "preview/__type_traits/void_t.h"

namespace preview {

template<typename T, typename = void>
struct is_referencable : std::false_type {};

template<typename T>
struct is_referencable<T, void_t<T&>> : std::true_type {};

template<typename T>
PREVIEW_INLINE_VARIABLE constexpr bool is_referencable_v = is_referencable<T>::value;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_IS_REFERENCEABLE_H_
