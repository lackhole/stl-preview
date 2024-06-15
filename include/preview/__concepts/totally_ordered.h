//
// Created by yonggyulee on 2023/12/25.
//

#ifndef PREVIEW_CONCEPTS_TOTALLY_ORDERED_H_
#define PREVIEW_CONCEPTS_TOTALLY_ORDERED_H_

#include <type_traits>

#include "preview/__concepts/equality_comparable.h"
#include "preview/__concepts/partially_ordered_with.h"
#include "preview/__type_traits/common_reference.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {

template<typename T>
struct totally_ordered
    : conjunction<
        equality_comparable<T>,
        partially_ordered_with<T, T>
      >{};


namespace detail {

template<
    typename T,
    typename U,
    bool = conjunction<
               totally_ordered<T>,
               totally_ordered<U>,
               equality_comparable_with<T, U>
           >::value /* false */
>
struct totally_ordered_with_impl : std::false_type {};

template<typename T, typename U>
struct totally_ordered_with_impl<T, U, true>
    : conjunction<
        totally_ordered<
            common_reference_t< const std::remove_reference_t<T>&,
                                const std::remove_reference_t<U>&> >,
        partially_ordered_with<T, U>
      > {};

} // namespace detail

template<typename T, typename U>
struct totally_ordered_with : detail::totally_ordered_with_impl<T, U> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_TOTALLY_ORDERED_H_
