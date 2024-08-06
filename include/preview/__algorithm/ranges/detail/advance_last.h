//
// Created by yonggyulee on 2024. 8. 7.
//

#ifndef PREVIEW_ALGORITHM_RANGES_DETAIL_ADVANCE_LAST_H_
#define PREVIEW_ALGORITHM_RANGES_DETAIL_ADVANCE_LAST_H_

#include <cassert>
#include <type_traits>
#include <utility>

#include "preview/core.h"
#include "preview/__concepts/assignable_from.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/next.h"
#include "preview/__iterator/random_access_iterator.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/distance.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/detail/tag.h"

namespace preview {
namespace ranges {
namespace detail {

struct advance_last_niebloid {
  using tag_1    = preview::detail::tag_1;
  using tag_2    = preview::detail::tag_2;
  using tag_3    = preview::detail::tag_3;
  using tag_else = preview::detail::tag_else;

  template<typename I, typename S, std::enable_if_t<sentinel_for<S, I>::value, int> = 0>
  constexpr void operator()(I& first, S last) const {
    using tag_t = preview::detail::conditional_tag<
        assignable_from<I&, S>,
        conjunction<sized_sentinel_for<S, I>, random_access_iterator<I>>
    >;
    advance_last_iterator_sentinel(first, std::move(last), tag_t{});
  }

  template<typename I, typename S, std::enable_if_t<sentinel_for<S, I>::value, int> = 0>
  constexpr void operator()(I& first, S last, iter_difference_t<I> distance) const {
    using tag_t = preview::detail::conditional_tag<
        assignable_from<I&, S>,
        random_access_iterator<I>,
        conjunction<sized_sentinel_for<S, I>, random_access_iterator<I>>
    >;
    advance_last_iterator_sentinel_distance(first, std::move(last), std::move(distance), tag_t{});
  }

 private:
  template<typename I, typename S>
  constexpr void advance_last_iterator_sentinel(I& first, S last, tag_1) const {
    first = std::move(last);
  }
  template<typename I, typename S>
  constexpr void advance_last_iterator_sentinel(I& first, S last, tag_2) const {
    first += ranges::distance(first, last);
  }
  template<typename I, typename S>
  constexpr void advance_last_iterator_sentinel(I& first, S last, tag_else) const {
    while (first != last)
      ++first;
  }

  template<typename I, typename S>
  constexpr void advance_last_iterator_sentinel_distance(I& first, S last, iter_difference_t<I>, tag_1) const {
    first = std::move(last);
  }
  template<typename I, typename S>
  constexpr void advance_last_iterator_sentinel_distance(I& first, S, iter_difference_t<I> distance, tag_2) const {
    first += distance;
  }
  template<typename I, typename S>
  constexpr void advance_last_iterator_sentinel_distance(I& first, S last, iter_difference_t<I>, tag_3) const {
    first += ranges::distance(first, last);
  }
  template<typename I, typename S>
  constexpr void advance_last_iterator_sentinel_distance(I& first, S, iter_difference_t<I> distance,  tag_else) const {
    for (;distance != 0; --distance)
      ++first;
  }
};

// Same as ranges::advance but does not consider negative `n`
PREVIEW_INLINE_VARIABLE constexpr advance_last_niebloid advance_last{};

} // namespace detail
} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_DETAIL_ADVANCE_LAST_H_
