//
// Created by YongGyu Lee on 3/27/24.
//

#ifndef PREVIEW_RANGES_OUTPUT_RANGE_H_
#define PREVIEW_RANGES_OUTPUT_RANGE_H_

#include "preview/__ranges/iterator_t.h"
#include "preview/__iterator/output_iterator.h"
#include "preview/__ranges/range.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename R, typename T, bool = range<R>::value /* true */>
struct output_range_impl : std::false_type {};

template<typename R, typename T>
struct output_range_impl<R, T, true> : output_iterator<iterator_t<R>, T> {};

} // namespace detail

template<typename R, typename T>
struct output_range : detail::output_range_impl<R, T> {};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_OUTPUT_RANGE_H_
