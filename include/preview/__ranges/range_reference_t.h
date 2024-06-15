//
// Created by yonggyulee on 2023/12/30.
//

#ifndef PREVIEW_RANGES_RANGE_REFERENCE_T_H_
#define PREVIEW_RANGES_RANGE_REFERENCE_T_H_

#include "preview/__iterator/iter_reference_t.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename I, bool = has_typename_type<iter_reference<I>>::value /* true */>
struct range_reference_impl_2 {
  using type = iter_reference_t<I>;
};
template<typename I>
struct range_reference_impl_2<I, false> {};

template<typename R, bool = has_typename_type<iterator<R>>::value /* true */>
struct range_reference_impl_1 : range_reference_impl_2<iterator_t<R>> {};
template<typename R>
struct range_reference_impl_1<R, false> {};

} // namespace ranges

template<typename R>
struct range_reference : detail::range_reference_impl_1<R> {};

template<typename R>
using range_reference_t = typename range_reference<R>::type;

} // namespace preview
} // namespace ranges

#endif // PREVIEW_RANGES_RANGE_REFERENCE_T_H_
