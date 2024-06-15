//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_SIZED_RANGE_H_
#define PREVIEW_RANGES_SIZED_RANGE_H_

#include <type_traits>

#include "preview/__ranges/range.h"
#include "preview/__ranges/size.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename T, bool = range<T>::value>
struct sized_range_impl : std::false_type {};

template<typename T>
struct sized_range_impl<T, true> : is_invocable<decltype(ranges::size), T&> {};

} // namespace detail

template<typename T>
struct sized_range : detail::sized_range_impl<T> {};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_SIZED_RANGE_H_
