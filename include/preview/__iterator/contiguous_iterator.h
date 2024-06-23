//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_ITERATOR_CONTIGUOUS_ITERATOR_H_
#define PREVIEW_ITERATOR_CONTIGUOUS_ITERATOR_H_

#include <type_traits>

#include "preview/__concepts/derived_from.h"
#include "preview/__concepts/same_as.h"
#include "preview/__concepts/requires_expression.h"
#include "preview/__iterator/detail/have_cxx20_iterator.h"
#include "preview/__iterator/detail/iter_concept.h"
#include "preview/__iterator/iterator_traits/legacy_random_access_iterator.h"
#include "preview/__iterator/iter_reference_t.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/random_access_iterator.h"
#include "preview/__memory/to_address.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename I, typename = void>
struct explicit_contiguous_requires : std::false_type {};
template<typename I>
struct explicit_contiguous_requires<
        I,
        void_t<decltype( preview::to_address( std::declval<I>() ) )>
    > : same_as<
        decltype( preview::to_address( std::declval<I>() ) ),
        std::add_pointer_t<iter_reference_t<remove_cvref_t<I>>>
    > {};

template<typename I>
struct contiguous_requires : requires_expression<explicit_contiguous_requires, const I&> {};

template<
    typename I,
    bool = random_access_iterator<I>::value /* false */
>
struct contiguous_iterator_impl : std::false_type {};

template<typename I>
struct contiguous_iterator_impl<I, true>
    : conjunction<
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
        disjunction<
#endif
            derived_from<ITER_CONCEPT<I>, contiguous_iterator_tag>,
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
            conjunction<
              derived_from<ITER_CONCEPT<I>, random_access_iterator_tag>,
              LegacyRandomAccessIterator<I>
            >
        >,
#endif
        std::is_lvalue_reference<iter_reference_t<I>>,
        same_as<iter_value_t<I>, remove_cvref_t<iter_reference_t<I>>>,
        contiguous_requires<I>
      > {};

} // namespace detail

template<typename I>
struct contiguous_iterator : detail::contiguous_iterator_impl<I> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_CONTIGUOUS_ITERATOR_H_
