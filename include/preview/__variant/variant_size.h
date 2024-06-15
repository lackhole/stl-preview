//
// Created by YongGyu Lee on 02/01/24.
//

#ifndef PREVIEW_VARIANT_VARIANT_SIZE_H_
#define PREVIEW_VARIANT_VARIANT_SIZE_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"

namespace preview {

template<typename... T>
class variant;

template<typename T>
struct variant_size;

template<typename... Ts>
struct variant_size<variant<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)> {};

template<typename T>
struct variant_size<const T> : std::integral_constant<std::size_t, variant_size<T>::value> {};

template<typename T>
PREVIEW_INLINE_VARIABLE constexpr std::size_t variant_size_v = variant_size<T>::value;

} // namespace preview

#endif // PREVIEW_VARIANT_VARIANT_SIZE_H_
