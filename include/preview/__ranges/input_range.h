//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_INPUT_RANGE_H_
#define PREVIEW_RANGES_INPUT_RANGE_H_

#include <type_traits>

#include "preview/__iterator/input_iterator.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename T, bool = has_typename_type<iterator<T>>::value>
struct input_range_impl : std::false_type {};

template<typename T>
struct input_range_impl<T, true>
    : conjunction<
        range<T>,
        input_iterator<iterator_t<T>>
      > {};

} // namespace detail

template<typename T>
struct input_range : detail::input_range_impl<T> {};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_INPUT_RANGE_H_
