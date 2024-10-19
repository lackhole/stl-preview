//
// Created by YongGyu Lee on 4/12/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_FIND_FIRST_OF_H_
#define PREVIEW_ALGORITHM_RANGES_FIND_FIRST_OF_H_

#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/indirectly_comparable.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__functional/equal_to.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace detail {

struct find_first_of_niebloid {
  template<typename I1, typename S1,
           typename I2, typename S2,
           typename Pred = ranges::equal_to,
           typename Proj1 = identity,
           typename Proj2 = identity,
           std::enable_if_t<conjunction<
               input_iterator<I1>, sentinel_for<S1, I1>,
               forward_iterator<I2>, sentinel_for<S2, I2>,
               indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
           >::value, int> = 0>
  constexpr I1 operator()(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    for (; first1 != last1; ++first1) {
      for (auto it = first2; it != last2; ++it) {
        if (preview::invoke(pred, preview::invoke(proj1, *first1), preview::invoke(proj2, *it))) {
          return first1;
        }
      }
    }

    return first1;
  }

  template<typename R1, typename R2,
           typename Pred = ranges::equal_to, typename Proj1 = identity, typename Proj2 = identity,
           std::enable_if_t<conjunction<
               input_range<R1>, forward_range<R2>,
               indirectly_comparable<iterator_t<R1>, iterator_t<R2>, Pred, Proj1, Proj2>
           >::value, int> = 0>
  constexpr borrowed_iterator_t<R1> operator()(R1&& r1, R2&& r2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    return (*this)(ranges::begin(r1), ranges::end(r1), ranges::begin(r2), ranges::end(r2),
                   preview::wrap_functor(pred), preview::wrap_functor(proj1), preview::wrap_functor(proj2));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::find_first_of_niebloid find_first_of{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_FIND_FIRST_OF_H_
