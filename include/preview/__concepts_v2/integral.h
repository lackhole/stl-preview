//
// Created by yonggyulee on 2024. 9. 4.
//

#ifndef PREVIEW_CONCEPTS_V2_INTEGRAL_H_
#define PREVIEW_CONCEPTS_V2_INTEGRAL_H_

#include <type_traits>

#include "preview/__concepts_v2/concept_base.h"
#include "preview/__type_traits/negation.h"

namespace preview {

#if defined(PREVIEW_USE_LEGACY_CONCEPT)

template<typename T> struct integral_c : concepts::concept_base<integral_c<T>, std::is_integral<T>> {};
template<typename T> PREVIEW_INLINE_VARIABLE constexpr integral_c<T> integral;

#else

template<typename T>
concept integral = std::is_integral_v<T>;

#endif

} // namespace preview

#endif // PREVIEW_CONCEPTS_V2_INTEGRAL_H_
