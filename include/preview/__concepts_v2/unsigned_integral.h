//
// Created by yonggyulee on 2024. 9. 4.
//

#ifndef PREVIEW_CONCEPTS_V2_UNSIGNED_INTEGRAL_H_
#define PREVIEW_CONCEPTS_V2_UNSIGNED_INTEGRAL_H_

#include <type_traits>

#include "preview/__concepts_v2/concept_base.h"
#include "preview/__type_traits/negation.h"

namespace preview {

#if defined(PREVIEW_USE_LEGACY_CONCEPT)

template<typename T> struct unsigned_integral_c : concepts::concept_base<unsigned_integral_c<T>, decltype(integral<T> && negation<std::is_signed<T>>{})> {};
template<typename T> PREVIEW_INLINE_VARIABLE constexpr unsigned_integral_c<T> unsigned_integral;

#else

template<typename T>
concept unsigned_integral = integral<T> && !std::is_signed_v<T>;

#endif

} // namespace preview

#endif // PREVIEW_CONCEPTS_V2_UNSIGNED_INTEGRAL_H_
