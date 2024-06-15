//
// Created by yonggyulee on 2024/01/27.
//

#ifndef PREVIEW_RANGES_RANGE_CONST_REFERENCE_T_H_
#define PREVIEW_RANGES_RANGE_CONST_REFERENCE_T_H_

#include "preview/__iterator/iter_const_reference_t.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/range.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename I, bool = has_typename_type<iter_const_reference<I>>::value /* true */>
struct range_const_reference_impl_2 {
  using type = iter_const_reference_t<I>;
};
template<typename I>
struct range_const_reference_impl_2<I, false> {};

template<typename R, bool = range<R>::value /* true */>
struct range_const_reference_impl : range_const_reference_impl_2<iterator_t<R>> {};
template<typename R>
struct range_const_reference_impl<R, false> {};

} // namespace ranges

template<typename R>
struct range_const_reference : detail::range_const_reference_impl<R> {};

template<typename R>
using range_const_reference_t = typename range_const_reference<R>::type;

} // namespace preview
} // namespace ranges

#endif // PREVIEW_RANGES_RANGE_CONST_REFERENCE_T_H_
