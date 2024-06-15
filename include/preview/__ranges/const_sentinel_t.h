//
// Created by cosge on 2024-01-27.
//

#ifndef PREVIEW_RANGES_CONST_SENTINEL_T_H_
#define PREVIEW_RANGES_CONST_SENTINEL_T_H_

#include <type_traits>

#include "preview/__ranges/cend.h"
#include "preview/__ranges/range.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename R, bool = range<R>::value, typename = void>
struct const_sentinel_impl {};

template<typename R>
struct const_sentinel_impl<R, true, void_t<decltype( ranges::cend(std::declval<R&>()) )>> {
  using type = decltype(ranges::cend(std::declval<R&>()));
};

} // namespace detail

template<typename R>
struct const_sentinel : detail::const_sentinel_impl<R> {};

template<typename R>
using const_sentinel_t = typename const_sentinel<R>::type;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_CONST_SENTINEL_T_H_
