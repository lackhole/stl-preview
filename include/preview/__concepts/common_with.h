//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_CONCEPTS_COMMON_WITH_H_
#define PREVIEW_CONCEPTS_COMMON_WITH_H_

#include <type_traits>

#include "preview/__concepts/common_reference_with.h"
#include "preview/__concepts/same_as.h"
#include "preview/__type_traits/common_reference.h"
#include "preview/__type_traits/common_type.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace detail {

template<
    typename T,
    typename U,
    bool = conjunction<
               same_as< common_type_t<T, U>, common_type_t<U, T> >,
               has_typename_type<
                   common_reference<std::add_lvalue_reference_t<const T>,
                                    std::add_lvalue_reference_t<const U>> >
           >::value /* false */
>
struct common_with_impl_2 : std::false_type {};

template<typename T, typename U>
struct common_with_impl_2<T, U, true>
    : conjunction<
          std::is_constructible< common_type_t<T, U>, decltype(std::declval<T>()) >,
          std::is_constructible< common_type_t<T, U>, decltype(std::declval<U>()) >,
          common_reference_with<
              std::add_lvalue_reference_t<const T>,
              std::add_lvalue_reference_t<const U>>,
          common_reference_with<
              std::add_lvalue_reference_t<common_type_t<T, U>>,
              common_reference_t<
                  std::add_lvalue_reference_t<const T>,
                  std::add_lvalue_reference_t<const U>
              >
          >
      > {};


template<
    typename T,
    typename U,
    bool = conjunction<has_typename_type< common_type<T, U> >,
                       has_typename_type< common_type<U, T> >
           >::value /* true */
>
struct common_with_impl_1 : common_with_impl_2<T, U> {};

template<typename T, typename U>
struct common_with_impl_1<T, U, false> : std::false_type {};

} // namespace detail

template<typename T, typename U>
struct common_with : detail::common_with_impl_1<T, U> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_COMMON_REFERENCE_WITH_H_
