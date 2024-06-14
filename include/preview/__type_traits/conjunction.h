//
// Created by cosge on 2023-10-14.
//

#ifndef PREVIEW_TYPE_TRAITS_CONJUNCTION_H_
#define PREVIEW_TYPE_TRAITS_CONJUNCTION_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"

namespace preview {

template<typename ...B>
struct conjunction;

template<>
struct conjunction<> : std::true_type {};

template<typename B1>
struct conjunction<B1> : B1 {};

template<typename B1, typename ...BN>
struct conjunction<B1, BN...> : std::conditional_t<bool(B1::value), conjunction<BN...>, B1> {};

template<typename... B>
PREVIEW_INLINE_VARIABLE constexpr bool conjunction_v = conjunction<B...>::value;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_CONJUNCTION_H_
