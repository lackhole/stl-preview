//
// Created by YongGyuLee on 2025-03-06.
//

#ifndef PREVIEW_ALGORITHM_RANGES_STARTS_WITH_H_
#define PREVIEW_ALGORITHM_RANGES_STARTS_WITH_H_

#include <type_traits>
#include <utility>

#include "preview/__algorithm/ranges/mismatch.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/equal_to.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/indirectly_comparable.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace detail {

struct starts_with_nebloid {
  template<
      typename I1, typename S1,
      typename I2, typename S2,
      typename Pred = ranges::equal_to,
      typename Proj1 = identity,
      typename Proj2 = identity,
      std::enable_if_t<conjunction_v<
          input_iterator<I1>, sentinel_for<S1, I1>,
          input_iterator<I2>, sentinel_for<S2, I2>,
          indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
  >, int> = 0>
  constexpr bool operator()(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    return ranges::mismatch(
        std::move(first1), last1,
        std::move(first2), last2,
        preview::wrap_functor(pred),
        preview::wrap_functor(proj1),
        preview::wrap_functor(proj2)
    ).in2 == last2;
  }

  template<
      typename R1, typename R2,
      typename Pred = ranges::equal_to,
      typename Proj1 = identity,
      typename Proj2 = identity,
      std::enable_if_t<conjunction_v<
          input_range<R1>, input_range<R2>,
          indirectly_comparable<ranges::iterator_t<R1>, ranges::iterator_t<R2>, Pred, Proj1, Proj2>
  >, int> = 0>
  constexpr bool operator()(R1&& r1, R2&& r2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
      return (*this)(ranges::begin(r1), ranges::end(r1),
                     ranges::begin(r2), ranges::end(r2),
                     preview::wrap_functor(pred),
                     preview::wrap_functor(proj1),
                     preview::wrap_functor(proj2));
    }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::starts_with_nebloid starts_with{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_STARTS_WITH_H_
