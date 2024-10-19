//
// Created by YongGyu Lee on 3/19/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_SET_INTERSECTION_H_
#define PREVIEW_ALGORITHM_RANGES_SET_INTERSECTION_H_

#include <utility>

#include "preview/__algorithm/ranges/in_in_out_result.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/less.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/mergeable.h"
#include "preview/__iterator/next.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__iterator/weakly_incrementable.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/input_range.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {

template< class I1, class I2, class O >
using set_intersection_result = ranges::in_in_out_result<I1, I2, O>;

namespace detail {

struct set_intersection_niebloid {
 private:
  template<typename R1, typename R2, typename O, typename Comp, typename Proj1, typename Proj2,
           bool = conjunction<input_range<R1>, input_range<R2>>::value /* false */>
  struct check_range : std::false_type {};
  template<typename R1, typename R2, typename O, typename Comp, typename Proj1, typename Proj2>
  struct check_range<R1, R2, O, Comp, Proj1, Proj2, true>
      : conjunction<
          weakly_incrementable<O>,
          mergeable<iterator_t<R1>, iterator_t<R2>, O, Comp, Proj1, Proj2>
      > {};

 public:
  template<
      typename I1, typename S1,
      typename I2, typename S2,
      typename O,
      typename Comp = ranges::less,
      typename Proj1 = identity,
      typename Proj2 = identity,
      std::enable_if_t<conjunction<
          input_iterator<I1>, sentinel_for<S1, I1>,
          input_iterator<I2>, sentinel_for<S2, I2>,
          weakly_incrementable<O>,
          mergeable<I1, I2, O, Comp, Proj1, Proj2>
      >::value, int> = 0>
  constexpr set_intersection_result<I1, I2, O>
  operator()(I1 first1, S1 last1, I2 first2, S2 last2, O result, Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    while (first1 != last1 && first2 != last2) {
      if (preview::invoke(comp, preview::invoke(proj1, *first1), preview::invoke(proj2, *first2))) {
        ++first1;
      } else if (preview::invoke(comp, preview::invoke(proj2, *first2), preview::invoke(proj1, *first1))) {
        ++first2;
      } else {
        *result = *first1;
        ++first1;
        ++first2;
        ++result;
      }
    }
    return {
      ranges::next(std::move(first1), std::move(last1)),
      ranges::next(std::move(first2), std::move(last2)),
      std::move(result)
    };
  }

  template<
      typename R1, typename R2,
      typename O,
      typename Comp = ranges::less,
      typename Proj1 = identity,
      typename Proj2 = identity,
      std::enable_if_t<
          check_range<R1, R2, O, Comp, Proj1, Proj2>
      ::value, int> = 0>
  constexpr set_intersection_result<borrowed_iterator_t<R1>, borrowed_iterator_t<R2>, O>
  operator()(R1&& r1, R2&& r2, O result, Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    return (*this)(ranges::begin(r1), ranges::end(r1),
                   ranges::begin(r2), ranges::end(r2),
                   std::move(result),
                   preview::wrap_functor(comp), preview::wrap_functor(proj1), preview::wrap_functor(proj2));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::set_intersection_niebloid set_intersection{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_SET_INTERSECTION_H_
