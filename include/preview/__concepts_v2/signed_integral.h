//
// Created by yonggyulee on 2024. 9. 4.
//

#ifndef PREVIEW_CONCEPTS_V2_SIGNED_INTEGRAL_H_
#define PREVIEW_CONCEPTS_V2_SIGNED_INTEGRAL_H_

#include <type_traits>

#include "preview/__concepts_v2/integral.h"

namespace preview {

#if defined(PREVIEW_USE_LEGACY_CONCEPT)

template<typename T> struct signed_integral_c : concepts::concept_base<signed_integral_c<T>, decltype(integral<T> && std::is_signed<T>{})> {};
template<typename T> PREVIEW_INLINE_VARIABLE constexpr signed_integral_c<T> signed_integral;

#else

template<typename T>
concept signed_integral = integral<T> && std::is_signed_v<T>;

#endif

} // namespace preview

#endif // PREVIEW_CONCEPTS_V2_SIGNED_INTEGRAL_H_
