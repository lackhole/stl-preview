//
// Created by YongGyu Lee on 5/8/24.
//

#ifndef PREVIEW_UTILITY_IN_RANGE_H_
#define PREVIEW_UTILITY_IN_RANGE_H_

#include <limits>

#include "preview/__utility/cmp.h"

namespace preview {

template<typename R, typename T>
constexpr bool in_range(T t) noexcept {
  return preview::cmp_greater_equal(t, (std::numeric_limits<R>::min)()) &&
         preview::cmp_less_equal(t, (std::numeric_limits<R>::max)());
}

} // namespace preview

#endif // PREVIEW_UTILITY_IN_RANGE_H_
