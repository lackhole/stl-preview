//
// Created by yonggyulee on 2024/01/22.
//

#ifndef PREVIEW_RANGES_RANGE_RVALUE_REFERENCE_T_H_
#define PREVIEW_RANGES_RANGE_RVALUE_REFERENCE_T_H_

#include "preview/__iterator/iter_rvalue_reference_t.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/range.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename R, bool = range<R>::value /* false */>
struct range_rvalue_reference_impl {};

template<typename R>
struct range_rvalue_reference_impl<R, true> {
 using type = iter_rvalue_reference_t<iterator_t<R>>;
};

} // namespace detail

template<typename R>
struct range_rvalue_reference : detail::range_rvalue_reference_impl<R> {};

template<typename R>
using range_rvalue_reference_t = typename range_rvalue_reference<R>::type;

} // namespace preview
} // namespace ranges

#endif // PREVIEW_RANGES_RANGE_RVALUE_REFERENCE_T_H_
