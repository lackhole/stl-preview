//
// Created by cosge on 2024-01-13.
//

#ifndef PREVIEW_ITERATOR_ITER_MAPPED_T_H_
#define PREVIEW_ITERATOR_ITER_MAPPED_T_H_

#include <tuple>
#include <type_traits>

#include "preview/__iterator/iter_val_t.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_complete.h"

namespace preview {
namespace detail {

template<typename V, bool = is_complete<std::tuple_element<1, V>>::value /* true */>
struct iter_mapped_impl_2 {
  using type = std::tuple_element_t<1, V>;
};
template<typename V>
struct iter_mapped_impl_2<V, false> {};

template<typename I, bool = has_typename_type< iter_val<I> >::value /* true */>
struct iter_mapped_impl : iter_mapped_impl_2<iter_val_t<I>> {};
template<typename I>
struct iter_mapped_impl<I, false> {};

} // namespace detail

template<typename T>
struct iter_mapped : detail::iter_mapped_impl<T> {};

template<typename T>
using iter_mapped_t = typename iter_mapped<T>::type;

} // namespace preview

#endif // PREVIEW_ITERATOR_ITER_MAPPED_T_H_
