//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_ITERATOR_INDIRECTLY_READABLE_TRAITS_H_
#define PREVIEW_ITERATOR_INDIRECTLY_READABLE_TRAITS_H_

#include <type_traits>

#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/no_traits.h"
#include "preview/__type_traits/has_typename_element_type.h"
#include "preview/__type_traits/has_typename_value_type.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace detail {

template<typename T>
struct value_typer {
  using value_type = T;
};

template<
    typename T,
    bool = has_typename_value_type<T>::value,
    bool = has_typename_element_type<T>::value
>
struct indirectly_readable_traits_impl {};

template<typename T>
struct indirectly_readable_traits_impl<T, true, false>
    : std::conditional_t<
        std::is_object<typename T::value_type>::value,
        value_typer<std::remove_cv_t<typename T::value_type>>,
        no_traits
      > {};

template<typename T>
struct indirectly_readable_traits_impl<T, false, true>
    : std::conditional_t<
        std::is_object<typename T::element_type>::value,
        value_typer<std::remove_cv_t<typename T::element_type>>,
        no_traits
      > {};

template<typename T>
struct indirectly_readable_traits_impl<T, true, true>
    : std::conditional_t<
        conjunction<
          std::is_object<typename T::value_type>, std::is_object<typename T::element_type>,
          std::is_same<std::remove_cv_t<typename T::value_type>, std::remove_cv_t<typename T::element_type>>
        >::value,
        value_typer<std::remove_cv_t<typename T::value_type>>,
        no_traits
      > {};

} // namespace detail

template<typename I>
struct indirectly_readable_traits : detail::indirectly_readable_traits_impl<I> {};

template<typename T>
struct indirectly_readable_traits<T*>
    : std::conditional_t<
        std::is_object<T>::value,
        detail::value_typer<remove_cvref_t<T>>,
        no_traits
      > {};

template<typename T>
struct indirectly_readable_traits<const T> : indirectly_readable_traits<T> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_INDIRECTLY_READABLE_TRAITS_H_
