//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_ITERATOR_ITER_DIFFERENCE_T_H_
#define PREVIEW_ITERATOR_ITER_DIFFERENCE_T_H_

#include "preview/__iterator/incrementable_traits.h"
#include "preview/__iterator/iterator_traits/forward_declare.h"
#include "preview/__type_traits/has_typename_difference_type.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace detail {

template<
    typename T,
    bool = is_primary_iterator_traits<iterator_traits<remove_cvref_t<T> > >::value,
    bool = has_typename_difference_type<incrementable_traits<remove_cvref_t<T>>>::value
>
struct iter_difference_impl {};

template<typename T, bool v>
struct iter_difference_impl<T, false, v> {
  using type = typename iterator_traits<remove_cvref_t<T>>::difference_type;
};

template<typename T>
struct iter_difference_impl<T, true, true> {
  using type = typename incrementable_traits<remove_cvref_t<T>>::difference_type;
};

} // namespace detail

template<typename T>
struct iter_difference : detail::iter_difference_impl<T> {};

template<typename T>
using iter_difference_t = typename iter_difference<T>::type;

} // namespace preview

#endif // PREVIEW_ITERATOR_ITER_DIFFERENCE_T_H_
