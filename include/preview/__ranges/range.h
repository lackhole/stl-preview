//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_RANGE_H_
#define PREVIEW_RANGES_RANGE_H_

#include <type_traits>

#include "preview/__ranges/begin.h"
#include "preview/__ranges/end.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/is_referenceable.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename T, bool = is_referencable<T>::value /* false */>
struct is_range : std::false_type {};

template<typename T>
struct is_range<T, true>
    : conjunction<
        is_invocable<decltype(ranges::begin), T&>,
        is_invocable<decltype(ranges::end), T&>
    > {};

} // namespace detail

template<typename T>
struct range : detail::is_range<T> {};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_RANGE_H_
