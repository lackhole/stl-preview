//
// Created by YongGyu Lee on 3/27/24.
//

#ifndef PREVIEW_RANGES_POSSIBLY_CONST_RANGE_H_
#define PREVIEW_RANGES_POSSIBLY_CONST_RANGE_H_

#include <type_traits>

#include "preview/__ranges/constant_range.h"
#include "preview/__ranges/input_range.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/negation.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename R>
constexpr auto& possibly_const_range_impl(R& r, std::true_type) noexcept {
  return const_cast<const R&>(r);
}

template<typename R>
constexpr auto& possibly_const_range_impl(R& r, std::false_type) noexcept {
  return r;
}

} // namespace detail

template<typename R, std::enable_if_t<input_range<R>::value, int> = 0>
constexpr auto& possibly_const_range(R& r) noexcept {
  return detail::possibly_const_range_impl(r, conjunction<constant_range<const R>, negation<constant_range<R>>>{});
}

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_POSSIBLY_CONST_RANGE_H_
