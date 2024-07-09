//
// Created by YongGyu Lee on 2024. 7. 5.
//

#ifndef PREVIEW_ALGORITHM_RANGES_ADJACENT_FIND_H_
#define PREVIEW_ALGORITHM_RANGES_ADJACENT_FIND_H_

#include <functional>
#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__functional/equal_to.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/indirect_binary_predicate.h"
#include "preview/__iterator/next.h"
#include "preview/__iterator/projected.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

struct adjacent_find_niebloid {
  template<typename I, typename S, typename Proj = identity, typename Pred = equal_to, std::enable_if_t<conjunction<
      forward_iterator<I>,
      sentinel_for<S, I>,
      projectable<I, Proj>,
      indirect_binary_predicate<
          Pred,
          projected<I, Proj>,
          projected<I, Proj>>
  >::value, int> = 0>
  constexpr I operator()(I first, S last, Pred pred = {}, Proj proj = {}) const {
    using namespace rel_ops;
    if (first == last)
      return last;
    auto next = ranges::next(first);
    for (; next != last; ++next, ++first) {
      if (preview::invoke(pred, preview::invoke(proj, *first), preview::invoke(proj, *next)))
        return first;
    }
    return next;
  }

  template<typename R, typename Proj = identity, typename Pred = equal_to, std::enable_if_t<conjunction<
      forward_range<R>,
      projectable<iterator_t<R>, Proj>,
      indirect_binary_predicate<
          Pred,
          projected<iterator_t<R>, Proj>,
          projected<iterator_t<R>, Proj>>
  >::value, int> = 0>
  constexpr borrowed_iterator_t<R> operator()(R&& r, Pred pred = {}, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), std::ref(pred), std::ref(proj));
  }
};

} // namespace detail

constexpr PREVIEW_INLINE_VARIABLE detail::adjacent_find_niebloid adjacent_find{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_ADJACENT_FIND_H_
