//
// Created by yonggyulee on 2024. 8. 14.
//

#ifndef PREVIEW_ALGORITHM_RANGES_COPY_BACKWARD_H_
#define PREVIEW_ALGORITHM_RANGES_COPY_BACKWARD_H_

#include <type_traits>
#include <utility>

#include "preview/__algorithm/ranges/in_out_result.h"
#include "preview/__core/inline_variable.h"
#include "preview/__iterator/bidirectional_iterator.h"
#include "preview/__iterator/indirectly_copyable.h"
#include "preview/__iterator/next.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/bidirectional_range.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {

template<typename I1, typename I2>
using copy_backward_result = ranges::in_out_result<I1, I2>;

namespace detail {

struct copy_backward_niebloid {
  template<typename I1, typename S1, typename I2, std::enable_if_t<conjunction<
      bidirectional_iterator<I1>,
      sentinel_for<S1, I1>,
      bidirectional_iterator<I2>,
      indirectly_copyable<I1, I2>
  >::value, int> = 0>
  constexpr copy_backward_result<I1, I2> operator()(I1 first, S1 last, I2 result) const {
    I1 last_it = ranges::next(first, std::move(last));
    for (I1 it = last_it; it != first; ) {
      *--result = *--it;
    }
    return {std::move(last_it), std::move(result)};
  }

  template<typename R, typename I, std::enable_if_t<conjunction<
      bidirectional_range<R>,
      bidirectional_iterator<I>,
      indirectly_copyable<ranges::iterator_t<R>, I>
  >::value, int> = 0>
  constexpr copy_backward_result<ranges::borrowed_iterator_t<R>, I> operator()(R&& r, I result) const {
    return (*this)(ranges::begin(r), ranges::end(r), std::move(result));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::copy_backward_niebloid copy_backward{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_COPY_BACKWARD_H_
