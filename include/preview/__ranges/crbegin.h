//
// Created by YongGyu Lee on 3/27/24.
//

#ifndef PREVIEW_RANGES_CRBEGIN_H_
#define PREVIEW_RANGES_CRBEGIN_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/basic_const_iterator.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__ranges/possibly_const_range.h"
#include "preview/__ranges/rbegin.h"
#include "preview/__type_traits/disjunction.h"

namespace preview {
namespace ranges {
namespace detail {

struct crbegin_niebloid {
  template<typename T, std::enable_if_t<disjunction<
      std::is_lvalue_reference<T>,
      enable_borrowed_range_t<std::remove_cv_t<T>>
  >::value, int> = 0>
  constexpr auto operator()(T&& t) const {
    return preview::const_iterator<decltype(ranges::rbegin(possibly_const_range(t)))>(ranges::rbegin(possibly_const_range(t)));
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::crbegin_niebloid crbegin{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_CRBEGIN_H_
