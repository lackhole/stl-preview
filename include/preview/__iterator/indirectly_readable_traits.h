//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_ITERATOR_INDIRECTLY_READABLE_TRAITS_H_
#define PREVIEW_ITERATOR_INDIRECTLY_READABLE_TRAITS_H_

#include <type_traits>

#include "preview/__concepts/same_as.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/no_traits.h"
#include "preview/__type_traits/has_typename_element_type.h"
#include "preview/__type_traits/has_typename_value_type.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace detail {

template<typename T, bool = /* false */ std::is_object<T>::value>
struct value_type_if_obj {};
template<typename T>
struct value_type_if_obj<T, true> {
  using value_type = std::remove_cv_t<T>;
};

template<typename T>
struct value_typer {
  using value_type = T;
};

template<
    typename T,
    bool = has_typename_value_type<T>::value, /* false */
    bool = has_typename_element_type<T>::value /* false */
>
struct indirectly_readable_traits_impl {};

template<typename T>
struct indirectly_readable_traits_impl<T, true, false>
    : value_type_if_obj<typename T::value_type> {};

template<typename T>
struct indirectly_readable_traits_impl<T, false, true>
    : value_type_if_obj<typename T::element_type> {};

template<typename T>
struct indirectly_readable_traits_impl<T, true, true>
    : std::conditional_t<
        same_as<std::remove_cv_t<typename T::value_type>,
                std::remove_cv_t<typename T::element_type>>::value,
        value_type_if_obj<typename T::value_type>,
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
