//
// Created by yonggyulee on 1/18/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_COUNT_IF_H_
#define PREVIEW_ALGORITHM_RANGES_COUNT_IF_H_

#include <functional>
#include <type_traits>

#include "preview/__algorithm/ranges/detail/check_input.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/indirect_unary_predicate.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/range_difference_t.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

struct count_if_niebloid {
  template<typename I, typename S, typename Proj = identity, typename Pred, std::enable_if_t<
      algo_check_unary_input_iterator<indirect_unary_predicate, I, S, Proj, Pred>
  ::value, int> = 0>
  constexpr iter_difference_t<I> operator()(I first, S last, Pred pred, Proj proj = {}) const {
    using namespace preview::rel_ops;

    iter_difference_t<I> counter = 0;

    for (; first != last; ++first) {
      if (preview::invoke(pred, preview::invoke(proj, *first))) {
        ++counter;
      }
    }

    return counter;
  }

  template<typename R, typename Proj = identity, typename Pred, std::enable_if_t<
      algo_check_unary_input_range<indirect_unary_predicate, R, Proj, Pred>
  ::value, int> = 0>
  constexpr range_difference_t<R>
  operator()(R&& r, Pred pred, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), preview::wrap_functor(pred), preview::wrap_functor(proj));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::count_if_niebloid count_if{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_COUNT_IF_H_
