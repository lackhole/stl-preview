//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_CONCEPTS_SWAPPABLE_H_
#define PREVIEW_CONCEPTS_SWAPPABLE_H_

#include <type_traits>

#include "preview/__concepts/swap.h"
#include "preview/__type_traits/is_referenceable.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename T, bool = is_referencable<T>::value, typename = void>
struct swappable_impl : std::false_type {};

template<typename T>
struct swappable_impl<T, true, void_t<decltype(ranges::swap(std::declval<T&>(), std::declval<T&>()))>>
    : std::true_type {};

} // namespace detail

template<typename T>
struct swappable : detail::swappable_impl<T> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_SWAPPABLE_H_
