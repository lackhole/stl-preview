//
// Created by yonggyulee on 2023/12/30.
//

#ifndef PREVIEW_RANGES_CONSTANT_RANGE_H_
#define PREVIEW_RANGES_CONSTANT_RANGE_H_

#include <type_traits>

#include "preview/__concepts/same_as.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/iter_const_reference_t.h"
#include "preview/__iterator/iter_reference_t.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/view.h"

namespace preview {
namespace ranges {
namespace detail {

template<
    typename T,
    bool = conjunction<
               input_iterator<T>,
               has_typename_type<iter_const_reference<T>>
           >::value /* true */
>
struct constant_range_constant_iterator : same_as<iter_const_reference_t<T>, iter_reference_t<T>> {};
template<typename T>
struct constant_range_constant_iterator<T, false> : std::false_type {};

template<typename T, bool = input_range<T>::value /* true */>
struct constant_range_impl : constant_range_constant_iterator<iterator_t<T>> {};
template<typename T>
struct constant_range_impl<T, false> : std::false_type {};

} // namespace detail

template<typename T>
struct constant_range : detail::constant_range_impl<T> {};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_CONSTANT_RANGE_H_
