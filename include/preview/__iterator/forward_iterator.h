//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_ITERATOR_FORWARD_ITERATOR_H_
#define PREVIEW_ITERATOR_FORWARD_ITERATOR_H_

#include <type_traits>

#include "preview/__concepts/derived_from.h"
#include "preview/__iterator/detail/iter_concept.h"
#include "preview/__iterator/incrementable.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace detail {

template<typename I, bool = input_iterator<I>::value>
struct forward_iterator_impl : std::false_type {};

template<typename I>
struct forward_iterator_impl<I, true>
    : conjunction<
        derived_from<ITER_CONCEPT<I>, forward_iterator_tag>,
        incrementable<I>,
        sentinel_for<I, I>
      > {};

} // namespace detail

template<typename I>
struct forward_iterator : detail::forward_iterator_impl<I> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_FORWARD_ITERATOR_H_
