//
// Created by yonggyulee on 2024. 9. 4.
//

#ifndef PREVIEW_CONCEPTS_V2_INTEGRAL_H_
#define PREVIEW_CONCEPTS_V2_INTEGRAL_H_

#include <type_traits>

#include "preview/__concepts_v2/detail/concept_base.h"
#include "preview/__type_traits/negation.h"

namespace preview {

namespace concepts { template<typename T> struct integral : concept_base<integral<T>, std::is_integral<T>> {}; }
template<typename T> PREVIEW_INLINE_VARIABLE constexpr concepts::integral<T> integral;

namespace concepts { template<typename T> struct signed_integral : concept_base<signed_integral<T>, decltype(integral<T>{} && std::is_signed<T>{})> {}; }
template<typename T> PREVIEW_INLINE_VARIABLE constexpr concepts::signed_integral<T> signed_integral;

namespace concepts { template<typename T> struct unsigned_integral : concept_base<unsigned_integral<T>, decltype(integral<T>{} && negation<std::is_signed<T>>{})> {}; }
template<typename T> PREVIEW_INLINE_VARIABLE constexpr concepts::unsigned_integral<T> unsigned_integral;


//PREVIEW_DEFINE_CONCEPT_UNDER(concepts,
//                             template<typename T>, signed_integral, signed_integral<T>,
//                             preview::integral<T> && std::is_signed<T>{});
} // namespace preview

#endif // PREVIEW_CONCEPTS_V2_INTEGRAL_H_
