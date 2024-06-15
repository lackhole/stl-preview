//
// Created by yonggyulee on 2023/12/30.
//

#ifndef PREVIEW_RANGES_RANGE_VALUE_T_H_
#define PREVIEW_RANGES_RANGE_VALUE_T_H_

#include "preview/__iterator/iter_value_t.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename I, bool = has_typename_type<iter_value<I>>::value /* true */>
struct range_value_impl_2 {
  using type = iter_value_t<I>;
};
template<typename I>
struct range_value_impl_2<I, false> {};

template<typename R, bool = has_typename_type<iterator<R>>::value /* true */>
struct range_value_impl_1 : range_value_impl_2<iterator_t<R>> {};
template<typename R>
struct range_value_impl_1<R, false> {};

} // namespace ranges

template<typename R>
struct range_value : detail::range_value_impl_1<R> {};

template<typename R>
using range_value_t = typename range_value<R>::type;

} // namespace preview
} // namespace ranges

#endif // PREVIEW_RANGES_RANGE_VALUE_T_H_
