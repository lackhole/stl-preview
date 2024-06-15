//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_CONCEPTS_ASSIGNABLE_FROM_H_
#define PREVIEW_CONCEPTS_ASSIGNABLE_FROM_H_

#include <type_traits>

#include "preview/__concepts/common_reference_with.h"
#include "preview/__concepts/same_as.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_referenceable.h"

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4244)
#endif

namespace preview {
namespace impl {

template<
    typename LHS, typename RHS,
    bool = std::is_assignable<LHS, RHS&& >::value /* true */>
struct assignable_from_requires
    : same_as<decltype( std::declval<LHS>() = std::declval<RHS>() ), LHS> {};

template<typename LHS, typename RHS>
struct assignable_from_requires<LHS, RHS, false> : std::false_type {};


template<typename LHS, typename RHS>
struct assignable_from_impl
    : conjunction<
        common_reference_with<
            const std::remove_reference_t<LHS>&,
            const std::remove_reference_t<RHS>&>,
        assignable_from_requires<LHS, RHS>
      > {};
} // namespace impl

template<typename LHS, typename RHS>
struct assignable_from
    : std::conditional_t<
        conjunction<
          std::is_lvalue_reference<LHS>,
          is_referencable< std::remove_reference_t<LHS> >,
          is_referencable< std::remove_reference_t<RHS> > >::value,
        impl::assignable_from_impl<LHS, RHS>,
        std::false_type
    > {};

} // namespace preview

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif // PREVIEW_CONCEPTS_ASSIGNABLE_FROM_H_
