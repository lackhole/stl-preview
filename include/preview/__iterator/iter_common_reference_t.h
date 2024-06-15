//
// Created by yonggyulee on 2023/12/28.
//

#ifndef PREVIEW_ITERATOR_ITER_COMMON_REFERENCE_T_H_
#define PREVIEW_ITERATOR_ITER_COMMON_REFERENCE_T_H_

#include "preview/__iterator/indirectly_readable.h"
#include "preview/__iterator/iter_reference_t.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__type_traits/common_reference.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace detail {

template<
    typename T,
    bool = has_typename_type<
               common_reference< iter_reference_t<T>,
                                 iter_value_t<T>&    >
           >::value /* true */
>
struct iter_common_reference_impl_2 {
  using type = common_reference_t<iter_reference_t<T>, iter_value_t<T>&>;
};
template<typename T>
struct iter_common_reference_impl_2<T, false> {};

template<typename T, bool = indirectly_readable<T>::value /* true */>
struct iter_common_reference_impl_1 : iter_common_reference_impl_2<T> {};
template<typename T>
struct iter_common_reference_impl_1<T, false> {};

} // namespace detail

template<typename T>
struct iter_common_reference : detail::iter_common_reference_impl_1<T> {};

template<typename T>
using iter_common_reference_t = typename iter_common_reference<T>::type;

} // namespace preview

#endif // PREVIEW_ITERATOR_ITER_COMMON_REFERENCE_T_H_
