//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_BIDIRECTIONAL_RANGE_H_
#define PREVIEW_RANGES_BIDIRECTIONAL_RANGE_H_

#include <type_traits>

#include "preview/__iterator/bidirectional_iterator.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace ranges {
namespace detail {

template<
    typename T,
    bool =
        conjunction<
          forward_range<T>,
          has_typename_type<iterator<T>>
        >::value
>
struct bidirectional_range_impl : std::false_type {};

template<typename T>
struct bidirectional_range_impl<T, true> : bidirectional_iterator<iterator_t<T>> {};

} // namespace ranges

template<typename T>
struct bidirectional_range : detail::bidirectional_range_impl<T> {};

} // namespace preview
} // namespace ranges

#endif // PREVIEW_RANGES_BIDIRECTIONAL_RANGE_H_
