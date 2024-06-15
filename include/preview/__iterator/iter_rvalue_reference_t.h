//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_ITERATOR_ITER_RVALUE_REFERENCE_T_H_
#define PREVIEW_ITERATOR_ITER_RVALUE_REFERENCE_T_H_

#include <type_traits>

#include "preview/__concepts/dereferenceable.h"
#include "preview/__iterator/iter_move.h"

namespace preview {
namespace detail {

template<typename T, bool v = dereferenceable<T>::value, typename = void>
struct iter_rvalue_reference_impl {};

template<typename T>
struct iter_rvalue_reference_impl<T, true, void_t<decltype(ranges::iter_move(std::declval<T&>()))>> {
  using type = decltype(ranges::iter_move(std::declval<T&>()));
};

} // namespace detail

template<typename T>
struct iter_rvalue_reference : detail::iter_rvalue_reference_impl<T> {};

template<typename T>
using iter_rvalue_reference_t = typename iter_rvalue_reference<T>::type;

} // namespace preview

#endif // PREVIEW_ITERATOR_ITER_RVALUE_REFERENCE_T_H_
