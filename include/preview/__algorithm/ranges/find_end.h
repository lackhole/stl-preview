//
// Created by YongGyu Lee on 4/12/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_FIND_END_H_
#define PREVIEW_ALGORITHM_RANGES_FIND_END_H_

#include <iterator>
#include <type_traits>
#include <utility>

#include "preview/__algorithm/ranges/search.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/equal_to.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/indirectly_comparable.h"
#include "preview/__iterator/next.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_subrange_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/subrange.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace detail {

struct find_end_niebloid {
  template<typename I1, typename S1,
           typename I2, typename S2,
           typename Pred = ranges::equal_to,
           typename Proj1 = identity, typename Proj2 = identity,
           std::enable_if_t<conjunction<
               forward_iterator<I1>, sentinel_for<S1, I1>,
               forward_iterator<I2>, sentinel_for<S2, I2>,
               indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
           >::value, int> = 0>
  constexpr subrange<I1> operator()(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    if (first2 == last2) {
      auto last_it = ranges::next(first1, last1);
      return {last_it, last_it};
    }

    auto result = ranges::search(std::move(first1), last1, first2, last2,
                                 preview::wrap_functor(pred), preview::wrap_functor(proj1), preview::wrap_functor(proj2));
    if (result.empty())
      return result;

    for (;;) {
      auto new_result = ranges::search(std::next(result.begin()), last1, first2, last2,
                                       preview::wrap_functor(pred), preview::wrap_functor(proj1), preview::wrap_functor(proj2));
      if (new_result.empty())
        return result;
      result = std::move(new_result);
    }
  }

  template<typename R1, typename R2,
           typename Pred = ranges::equal_to, typename Proj1 = identity, typename Proj2 = identity,
           std::enable_if_t<conjunction<
               forward_range<R1>, forward_range<R2>,
               indirectly_comparable<iterator_t<R1>, iterator_t<R2>, Pred, Proj1, Proj2>
           >::value, int> = 0>
  constexpr borrowed_subrange_t<R1> operator()(R1&& r1, R2&& r2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    return (*this)(ranges::begin(r1), ranges::end(r1), ranges::begin(r2), ranges::end(r2),
                   preview::wrap_functor(pred), preview::wrap_functor(proj1), preview::wrap_functor(proj2));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::find_end_niebloid find_end{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_FIND_END_H_
