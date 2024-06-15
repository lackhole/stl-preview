//
// Created by yonggyulee on 2023/09/29.
//

#ifndef PREVIEW_TYPE_TRAITS_IS_BOUNDED_ARRAY_H_
#define PREVIEW_TYPE_TRAITS_IS_BOUNDED_ARRAY_H_

#include <cstddef>
#include <type_traits>

#include "preview/__core/inline_variable.h"

namespace preview {

template<class T>
struct is_bounded_array : std::false_type {};

template<class T, std::size_t N>
struct is_bounded_array<T[N]> : std::true_type {};

template<typename... B>
PREVIEW_INLINE_VARIABLE constexpr bool is_bounded_array_v = is_bounded_array<B...>::value;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_IS_BOUNDED_ARRAY_H_
