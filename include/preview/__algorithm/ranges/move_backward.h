//
// Created by yonggyulee on 09/05/2025
//

#ifndef PREVIEW_ALGORITHM_RANGES_MOVE_BACKWARD_H_
#define PREVIEW_ALGORITHM_RANGES_MOVE_BACKWARD_H_

#include <type_traits>
#include <utility>

#include "preview/config.h"
#include "preview/__algorithm/ranges/in_out_result.h"
#include "preview/__core/inline_variable.h"
#include "preview/__iterator/bidirectional_iterator.h"
#include "preview/__iterator/indirectly_movable.h"
#include "preview/__iterator/iter_move.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__iterator/weakly_incrementable.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/bidirectional_range.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {

template<typename I, typename O>
using move_backward_result = in_out_result<I, O>;

namespace detail {

struct move_backward_niebloid {
  template<typename I1, typename S1, typename I2, std::enable_if_t<conjunction_v<
      bidirectional_iterator<I1>,
      sentinel_for<S1, I1>,
      bidirectional_iterator<I2>,
      // requires
      indirectly_movable<I1, I2>
  >, int> = 0>
  constexpr move_backward_result<I1, I2> operator()(I1 first, S1 last, I2 result) const {
#if !PREVIEW_CONFORM_CXX20_STANDARD
    using namespace preview::rel_ops;
#endif

    while (first != last) {
      *--result = ranges::iter_move(--last);
    }
    return {std::move(first), std::move(result)};
  }

  template<typename R, typename I, std::enable_if_t<conjunction_v<
      bidirectional_range<R>,
      bidirectional_iterator<I>,
      // requires
      indirectly_movable<iterator_t<R>, I>
  >, int> = 0>
  constexpr move_backward_result<borrowed_iterator_t<R>, I> operator()(R&& r, I result) const {
    return (*this)(ranges::begin(r), ranges::end(r), std::move(result));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::move_backward_niebloid move_backward{};

} // namespace ranges
} // namespace preview 

#endif // PREVIEW_ALGORITHM_RANGES_MOVE_BACKWARD_H_
