//
// Created by cosge on 2024-01-13.
//

#ifndef PREVIEW_ITERATOR_ITER_VAL_T_H_
#define PREVIEW_ITERATOR_ITER_VAL_T_H_

#include "preview/__iterator/iterator_traits.h"
#include "preview/__type_traits/has_typename_value_type.h"

namespace preview {
namespace detail {

template<typename I, bool = has_typename_value_type<iterator_traits<I> >::value /* true */>
struct iter_val_impl {
  using type = typename iterator_traits<I>::value_type;
};
template<typename I>
struct iter_val_impl<I, false> {};

} // namespace detail

template<typename T>
struct iter_val : detail::iter_val_impl<T> {};

template<typename T>
using iter_val_t = typename iter_val<T>::type;

} // namespace preview

#endif // PREVIEW_ITERATOR_ITER_VAL_T_H_
