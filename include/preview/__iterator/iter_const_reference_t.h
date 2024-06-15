//
// Created by cosge on 2023-12-31.
//

#ifndef PREVIEW_ITERATOR_ITER_CONST_REFERENCE_T_H_
#define PREVIEW_ITERATOR_ITER_CONST_REFERENCE_T_H_

#include "preview/__iterator/indirectly_readable.h"
#include "preview/__iterator/iter_reference_t.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__type_traits/common_reference.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace detail {

template<
    typename T,
    bool v = has_typename_type<
                 common_reference<
                     const iter_value_t<T>&&,
                     iter_reference_t<T>
                 >
             >::value /* true */
>
struct iter_const_reference_impl_2 {
  using type = common_reference_t<const iter_value_t<T>&&, iter_reference_t<T>>;
};
template<typename T>
struct iter_const_reference_impl_2<T, false> {};


template<typename T, bool v = indirectly_readable<T>::value /* true */>
struct iter_const_reference_impl_1 : iter_const_reference_impl_2<T> {};
template<typename T>
struct iter_const_reference_impl_1<T, false> {};

} // namespace detail

template<typename T>
struct iter_const_reference : detail::iter_const_reference_impl_1<T> {};

template<typename T>
using iter_const_reference_t = typename iter_const_reference<T>::type;

} // namespace preview

#endif // PREVIEW_ITERATOR_ITER_CONST_REFERENCE_T_H_
