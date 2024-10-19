//
// Created by yonggyulee on 1/26/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_ALL_OF_H_
#define PREVIEW_ALGORITHM_RANGES_ALL_OF_H_

#include <type_traits>

#include "preview/core.h"
#include "preview/__algorithm/ranges/detail/check_input.h"
#include "preview/__algorithm/ranges/find_if_not.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/indirect_unary_predicate.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/end.h"

namespace preview {
namespace ranges {
namespace detail {

struct all_of_niebloid {
  template<typename I, typename S, typename Proj = identity, typename Pred, std::enable_if_t<
      algo_check_unary_input_iterator<indirect_unary_predicate, I, S, Proj, Pred>
  ::value, int> = 0>
  constexpr bool operator()(I first, S last, Pred pred, Proj proj = {}) const {
    return ranges::find_if_not(first, last, preview::wrap_functor(pred), preview::wrap_functor(proj)) == last;
  }

  template<typename R, typename Proj = identity, typename Pred, std::enable_if_t<
      algo_check_unary_input_range<indirect_unary_predicate, R, Proj, Pred>
  ::value, int> = 0>
  constexpr bool operator()(R&& r, Pred pred, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), preview::wrap_functor(pred), preview::wrap_functor(proj));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::all_of_niebloid all_of{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_ALL_OF_H_
