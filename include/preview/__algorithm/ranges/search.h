//
// Created by yonggyulee on 2/3/24.
//

#ifndef PREVIEW_ALGORITHM_SEARCH_H_
#define PREVIEW_ALGORITHM_SEARCH_H_

#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__functional/equal_to.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/indirectly_comparable.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_subrange_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/subrange.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace detail {

struct search_niebloid {
 private:
  template<typename R1, typename R2, typename Pred, typename Proj1, typename Proj2,
           bool = conjunction<forward_range<R1>, forward_range<R2>>::value /* false */>
  struct check_range : std::false_type {};
  template<typename R1, typename R2, typename Pred, typename Proj1, typename Proj2>
  struct check_range<R1, R2, Pred, Proj1, Proj2, true>
      : indirectly_comparable<iterator_t<R1>, iterator_t<R2>, Pred, Proj1, Proj2> {};

 public:
  template<
      typename I1, typename S1,
      typename I2, typename S2,
      typename Pred = equal_to, typename Proj1 = identity, typename Proj2 = identity,
      std::enable_if_t<conjunction<
          forward_iterator<I1>, sentinel_for<S1, I1>,
          forward_iterator<I2>, sentinel_for<S2, I2>,
          indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
      >::value, int> = 0
  >
  constexpr subrange<I1>
  operator()(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    for (;; ++first1) {
      I1 it1 = first1;
      I2 it2 = first2;
      for (;; ++it1, (void)++it2) {
        if (it2 == last2)
          return {first1, it1};
        if (it1 == last1)
          return {it1, it1};
        if (!preview::invoke(pred, preview::invoke(proj1, *it1), preview::invoke(proj2, *it2)))
          break;
      }
    }

    // unreachable
    return {last1, last1};
  }

  template<
      typename R1, typename R2,
      typename Pred = equal_to, typename Proj1 = identity, typename Proj2 = identity,
      std::enable_if_t<check_range<R1, R2, Pred, Proj1, Proj2>::value, int> = 0
  >
  constexpr borrowed_subrange_t<R1>
  operator()(R1&& r1, R2&& r2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    return (*this)(ranges::begin(r1), ranges::end(r1),
                   ranges::begin(r2), ranges::end(r2),
                   preview::wrap_functor(pred), preview::wrap_functor(proj1), preview::wrap_functor(proj2));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::search_niebloid search{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_SEARCH_H_
