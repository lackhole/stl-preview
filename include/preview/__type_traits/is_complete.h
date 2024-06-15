//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_TYPE_TRAITS_IS_COMPLETE_H_
#define PREVIEW_TYPE_TRAITS_IS_COMPLETE_H_

#include <cstddef>
#include <type_traits>

#include "preview/core.h"

namespace preview {
namespace detail {
template <class T, std::size_t = sizeof(T)>
std::true_type is_complete_impl(T *);

std::false_type is_complete_impl(...);
} // namespace detail

template <class T>
using is_complete = decltype(detail::is_complete_impl(std::declval<T*>()));

template<typename T>
PREVIEW_INLINE_VARIABLE constexpr bool is_complete_v = is_complete<T>::value;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_IS_COMPLETE_H_
