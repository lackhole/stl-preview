//
// Created by yonggyulee on 2023/12/26.
//

#ifndef PREVIEW_RANGES_RANDOM_ACCESS_RANGE_H_
#define PREVIEW_RANGES_RANDOM_ACCESS_RANGE_H_

#include <type_traits>

#include "preview/__iterator/random_access_iterator.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/bidirectional_range.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace ranges {
namespace detail {

template<
    typename T,
    bool =
        conjunction<
          bidirectional_range<T>,
          has_typename_type<iterator<T>>
        >::value
>
struct random_access_range_impl : std::false_type {};

template<typename T>
struct random_access_range_impl<T, true> : random_access_iterator<iterator_t<T>> {};

} // namespace ranges

template<typename T>
struct random_access_range : detail::random_access_range_impl<T> {};

} // namespace preview
} // namespace ranges

#endif // PREVIEW_RANGES_RANDOM_ACCESS_RANGE_H_
