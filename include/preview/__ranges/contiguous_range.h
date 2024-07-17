//
// Created by yonggyulee on 2023/12/30.
//

#ifndef PREVIEW_RANGES_CONTIGUOUS_RANGE_H_
#define PREVIEW_RANGES_CONTIGUOUS_RANGE_H_

#include <type_traits>

#include "preview/__concepts/same_as.h"
#include "preview/__iterator/contiguous_iterator.h"
#include "preview/__ranges/data.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/random_access_range.h"
#include "preview/__ranges/range_reference_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_invocable.h"

namespace preview {
namespace ranges {
namespace detail {

template<
    typename T,
    bool =
        conjunction<
            contiguous_iterator< iterator_t<T> >,
            is_invocable<decltype(ranges::data), T&>
        >::value /* true */
>
struct contiguous_range_impl_2
    : same_as<decltype( ranges::data(std::declval<T&>()) ), std::add_pointer_t<range_reference_t<T>>> {};
template<typename T>
struct contiguous_range_impl_2<T, false> : std::false_type {};

template<typename T, bool = /* true */ random_access_range<T>::value>
struct contiguous_range_impl_1 : contiguous_range_impl_2<T> {};
template<typename T>
struct contiguous_range_impl_1<T, false> : std::false_type {};

} // namespace ranges

template<typename T>
struct contiguous_range : detail::contiguous_range_impl_1<T> {};

} // namespace preview
} // namespace ranges

#endif // PREVIEW_RANGES_CONTIGUOUS_RANGE_H_
