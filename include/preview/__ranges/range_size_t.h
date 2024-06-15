//
// Created by yonggyulee on 2024/01/06.
//

#ifndef PREVIEW_RANGES_RANGE_SIZE_T_H_
#define PREVIEW_RANGES_RANGE_SIZE_T_H_

#include <type_traits>

#include "preview/__ranges/size.h"
#include "preview/__ranges/sized_range.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename R, bool = sized_range<R>::value /* true */>
struct range_size_impl {
  using type = decltype(ranges::size(std::declval<R&>()));
};

template<typename R>
struct range_size_impl<R, false> {};

} // namespace detail

template<typename R>
struct range_size : detail::range_size_impl<R> {};

template<typename R>
using range_size_t = typename range_size<R>::type;

} // namespace preview
} // namespace ranges

#endif // PREVIEW_RANGES_RANGE_SIZE_T_H_
