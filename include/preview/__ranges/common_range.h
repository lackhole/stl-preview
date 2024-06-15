//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_COMMON_RANGE_H_
#define PREVIEW_RANGES_COMMON_RANGE_H_

#include <type_traits>

#include "preview/__concepts/same_as.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/sentinel_t.h"
#include "preview/__ranges/range.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace ranges {
namespace detail {

template<
    typename T,
    bool =
        conjunction<
          range<T>,
          has_typename_type<iterator<T>>,
          has_typename_type<sentinel<T>>
        >::value
>
struct common_range_impl : std::false_type {};

template<typename T>
struct common_range_impl<T, true> : same_as<iterator_t<T>, sentinel_t<T>> {};

} // namespace ranges

template<typename T>
struct common_range : detail::common_range_impl<T> {};

} // namespace preview
} // namespace ranges

#endif // PREVIEW_RANGES_COMMON_RANGE_H_
