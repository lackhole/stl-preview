//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_ITERATOR_ITER_REFERENCE_T_H_
#define PREVIEW_ITERATOR_ITER_REFERENCE_T_H_

#include "preview/__concepts/dereferenceable.h"

namespace preview {
namespace detail {

template<typename T, bool v = dereferenceable<T>::value>
struct iter_reference_impl {};

template<typename T>
struct iter_reference_impl<T, true> {
  using type = decltype(*std::declval<T&>());
};

} // namespace detail

template<typename T>
struct iter_reference : detail::iter_reference_impl<T> {};

template<typename T>
using iter_reference_t = typename iter_reference<T>::type;

} // namespace preview

#endif // PREVIEW_ITERATOR_ITER_REFERENCE_T_H_
