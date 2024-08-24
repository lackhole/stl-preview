//
// Created by yonggyulee on 2024. 8. 24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_FILL_H_
#define PREVIEW_ALGORITHM_RANGES_FILL_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__iterator/output_iterator.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/output_range.h"
#include "preview/__ranges/range_value_t.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace detail {

struct fill_niebloid {
  template<typename O, typename S, typename T = iter_value_t<O>, std::enable_if_t<conjunction<
      sentinel_for<S, O>,
      output_iterator<O, const T&>
  >::value, int> = 0>
  constexpr O operator()(O first, S last, const T& value) const {
    // Must not use std::memset because it may be optimized away by the compiler
    while (first != last)
      *first++ = value;
    return first;
  }

  template<typename R, typename T = range_value_t<R>, std::enable_if_t<
      output_range<R, const T&>
  ::value, int> = 0>
  constexpr borrowed_iterator_t<R> operator()(R&& r, const T& value) const {
    return (*this)(ranges::begin(r), ranges::end(r), value);
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::fill_niebloid fill{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_FILL_H_
