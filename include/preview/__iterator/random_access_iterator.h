//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_ITERATOR_RANDOM_ACCESS_ITERATOR_H_
#define PREVIEW_ITERATOR_RANDOM_ACCESS_ITERATOR_H_

#include <type_traits>

#include "preview/__concepts/derived_from.h"
#include "preview/__concepts/implicit_expression_check.h"
#include "preview/__concepts/totally_ordered.h"
#include "preview/__iterator/detail/iter_concept.h"
#include "preview/__iterator/bidirectional_iterator.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/iter_reference_t.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_referenceable.h"
#include "preview/__type_traits/remove_cvref.h"

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4244)
#endif

namespace preview {
namespace detail {
namespace detail_random_access_iterator {

template<typename I, typename D, typename = void, typename = void>
struct explicit_op_assign_check : std::false_type {};
template<typename I, typename D>
struct explicit_op_assign_check<
      I, D,
      void_t<decltype( std::declval<I>() += std::declval<D>() )>,
      void_t<decltype( std::declval<I>() -= std::declval<D>() )>
    >
    : conjunction<
        same_as<decltype( std::declval<I>() += std::declval<D>() ), std::add_lvalue_reference_t<std::remove_reference_t<I>>>,
        same_as<decltype( std::declval<I>() -= std::declval<D>() ), std::add_lvalue_reference_t<std::remove_reference_t<I>>>
      > {};

template<typename I, typename D, typename = void, typename = void, typename = void, typename = void>
struct explicit_op_const_check : std::false_type {};
template<typename I, typename D>
struct explicit_op_const_check<
      I, D,
      void_t<decltype( std::declval<I>() + std::declval<D>() )>,
      void_t<decltype( std::declval<D>() + std::declval<I>() )>,
      void_t<decltype( std::declval<I>() - std::declval<D>() )>,
      void_t<decltype( std::declval<I>()[std::declval<D>()] )>
    >
    : conjunction<
        same_as<decltype( std::declval<I>() + std::declval<D>() ), remove_cvref_t<I>>,
        same_as<decltype( std::declval<D>() + std::declval<I>() ), remove_cvref_t<I>>,
        same_as<decltype( std::declval<I>() - std::declval<D>() ), remove_cvref_t<I>>,
        same_as<decltype( std::declval<I>()[std::declval<D>()] ), iter_reference_t<remove_cvref_t<I>>>
      > {};

template<typename I, bool = is_referencable<iter_difference_t<I>>::value /* false */>
struct op_check : std::false_type {};

template<typename I>
struct op_check<I, true>
    : conjunction<
        implicit_expression_check<explicit_op_assign_check, I&, const iter_difference_t<I>&>,
        implicit_expression_check<explicit_op_const_check, const I&, const iter_difference_t<I>&>
      > {};

} // detail_random_access_iterator

template<
    typename I,
    bool =
        conjunction<
          bidirectional_iterator<I>,
          has_typename_type<iter_difference<I>>,
          has_typename_type<iter_reference<I>>
        >::value /* false */
>
struct random_access_iterator_impl : std::false_type {};

template<typename I>
struct random_access_iterator_impl<I, true>
    : conjunction<
        derived_from<ITER_CONCEPT<I>, random_access_iterator_tag>,
        totally_ordered<I>,
        sized_sentinel_for<I, I>,
        detail_random_access_iterator::op_check<I>
      > {};

} // namespace detail

template<typename I>
struct random_access_iterator : detail::random_access_iterator_impl<I> {};

} // namespace preview

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif // PREVIEW_ITERATOR_RANDOM_ACCESS_ITERATOR_H_
