//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_ITERATOR_RANDOM_ACCESS_ITERATOR_H_
#define PREVIEW_ITERATOR_RANDOM_ACCESS_ITERATOR_H_

#include <type_traits>

#include "preview/__concepts/derived_from.h"
#include "preview/__concepts/requires_expression.h"
#include "preview/__concepts/subtractable.h"
#include "preview/__concepts/totally_ordered.h"
#include "preview/__iterator/detail/iter_concept.h"
#include "preview/__iterator/bidirectional_iterator.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/iter_reference_t.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_addible.h"
#include "preview/__type_traits/is_add_assignable.h"
#include "preview/__type_traits/is_subscriptable.h"
#include "preview/__type_traits/is_subtractable.h"
#include "preview/__type_traits/is_subtract_assignable.h"
#include "preview/__type_traits/is_referenceable.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/void_t.h"

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4244)
#endif

namespace preview {
namespace detail {

template<typename I, bool = conjunction<
    bidirectional_iterator<I>,
    has_typename_type<iter_difference<I>, is_referencable>,
    has_typename_type<iter_reference<I>>
>::value /* false */>
struct random_access_iterator_impl : std::false_type {};

template<typename I>
struct random_access_iterator_impl<I, true> :
#if defined(_MSC_VER) && _MSC_VER < 1920
    // BUGBUG
    // const T*& + const std::ptrdiff_t& -> T*
    // const std::ptrdiff_t& + const T*& -> T* const
    disjunction<
        std::is_pointer<std::decay_t<I>>,
#endif
    conjunction<
        derived_from<ITER_CONCEPT<I>, random_access_iterator_tag>,
        totally_ordered<I>,
        sized_sentinel_for<I, I>,
        is_add_assignable     <I&,       const iter_difference_t<I>&, same_as, I&>,
        is_subtract_assignable<I&,       const iter_difference_t<I>&, same_as, I&>,
        is_addible            <const I&, const iter_difference_t<I>&, same_as, I>,
        is_addible            <const iter_difference_t<I>&, const I&, same_as, I>,
        is_subtractable       <const I&, const iter_difference_t<I>&, same_as, I>,
        is_subscriptable      <const I&, const iter_difference_t<I>&, same_as, iter_reference_t<I>>
#if defined(_MSC_VER) && _MSC_VER < 1920
    >
#endif
    > {};

} // namespace detail

template<typename I>
struct random_access_iterator : detail::random_access_iterator_impl<I> {};

} // namespace preview

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif // PREVIEW_ITERATOR_RANDOM_ACCESS_ITERATOR_H_
