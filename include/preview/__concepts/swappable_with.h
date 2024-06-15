//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_CONCEPTS_SWAPPABLE_WITH_H_
#define PREVIEW_CONCEPTS_SWAPPABLE_WITH_H_

#include <type_traits>

#include "preview/__concepts/common_reference_with.h"
#include "preview/__concepts/swap.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename T, typename U, typename = void>
struct swap_uref_test : std::false_type {};

template<typename T, typename U>
struct swap_uref_test<T, U, void_t<decltype( ranges::swap(std::declval<T>(), std::declval<U>()) )>> : std::true_type {};

template<typename T, typename U, bool = common_reference_with<T, U>::value /* false */>
struct swappable_with_impl : std::false_type {};

template<typename T, typename U>
struct swappable_with_impl<T, U, true>
    : conjunction<
          swap_uref_test<T, T>,
          swap_uref_test<U, U>,
          swap_uref_test<T, U>,
          swap_uref_test<U, T>
      > {};

} // namespace detail

template<typename T, typename U>
struct swappable_with : detail::swappable_with_impl<T, U> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_SWAPPABLE_WITH_H_
