//
// Created by yonggyulee on 1/25/24.
//

#ifndef PREVIEW_CONCEPTS_VALUE_SWAPPABLE_H_
#define PREVIEW_CONCEPTS_VALUE_SWAPPABLE_H_

#include <type_traits>

#include "preview/__concepts/dereferenceable.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_swappable.h"

namespace preview {
namespace detail {

template<typename I1, typename I2, bool = conjunction<dereferenceable<I1>, dereferenceable<I2>>::value>
struct ValueSwappableImpl : std::false_type {};
template<typename I1, typename I2>
struct ValueSwappableImpl<I1, I2, true>
    : is_swappable<decltype(*std::declval<I1&>(), *std::declval<I2&>())> {};

} // namespace detail

template<typename I1, typename I2>
struct ValueSwappable : detail::ValueSwappableImpl<I1, I2> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_VALUE_SWAPPABLE_H_
