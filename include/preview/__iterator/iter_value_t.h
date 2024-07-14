//
// Created by cosge on 2023-12-24.
//

#ifndef PREVIEW_ITERATOR_ITER_VALUE_T_H_
#define PREVIEW_ITERATOR_ITER_VALUE_T_H_

#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/indirectly_readable_traits.h"
#include "preview/__type_traits/has_typename_value_type.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace detail {

template<
    typename T,
    bool = is_primary_iterator_traits<iterator_traits<remove_cvref_t<T> > >::value,
    bool = has_typename_value_type<indirectly_readable_traits<remove_cvref_t<T>>>::value
>
struct iter_value_impl {};

template<typename T, bool v>
struct iter_value_impl<T, true, v> {
  using type = typename indirectly_readable_traits<remove_cvref_t<T>>::value_type;
};

template<typename T>
struct iter_value_impl<T, false, true> {
  using type = typename iterator_traits<remove_cvref_t<T>>::value_type;
};

} // namespace detail

template<typename T>
struct iter_value : detail::iter_value_impl<T> {};

template<typename T>
using iter_value_t = typename iter_value<T>::type;

} // namespace preview

#endif // PREVIEW_ITERATOR_ITER_VALUE_T_H_
