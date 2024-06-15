//
// Created by YongGyu Lee on 2/24/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_LEXICOGRAPHICAL_COMPARE_H_
#define PREVIEW_ALGORITHM_RANGES_LEXICOGRAPHICAL_COMPARE_H_

#include <functional>
#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/less.h"
#include "preview/__iterator/indirect_strict_weak_order.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/projected.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/end.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

struct lexicographical_compare_niebloid {
 private:
  template<typename I1, typename Proj1, typename I2, typename Proj2, typename Comp,
           bool = conjunction<projectable<I1, Proj1>, projectable<I2, Proj2>>::value /* false_type */>
  struct test_projectable_iterator : std::false_type {};
  template<typename I1, typename Proj1, typename I2, typename Proj2, typename Comp>
  struct test_projectable_iterator<I1, Proj1, I2, Proj2, Comp, true>
      : indirect_strict_weak_order<Comp, projected<I1, Proj1>, projected<I2, Proj2>> {};

  template<typename R1, typename Proj1, typename R2, typename Proj2, typename Comp,
           bool = conjunction<input_range<R1>, input_range<R2>>::value /* false_type */>
  struct test_projectable_range : std::false_type {};
  template<typename R1, typename Proj1, typename R2, typename Proj2, typename Comp>
  struct test_projectable_range<R1, Proj1, R2, Proj2, Comp, true>
      : test_projectable_iterator<iterator_t<R1>, Proj1, iterator_t<R2>, Proj2, Comp> {};

 public:
  template<
      typename I1, typename S1,
      typename I2, typename S2,
      typename Proj1 = identity, typename Proj2 = identity, typename Comp = ranges::less,
      std::enable_if_t<conjunction<
          input_iterator<I1>, sentinel_for<S1, I1>,
          input_iterator<I2>, sentinel_for<S2, I2>,
          test_projectable_iterator<I1, Proj1, I2, Proj2, Comp>
      >::value, int> = 0
  >
  constexpr bool operator()(I1 first1, S1 last1, I2 first2, S2 last2, Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    using namespace rel_ops;
    for (; (first1 != last1) && (first2 != last2); ++first1, (void) ++first2) {
      if (preview::invoke(comp, preview::invoke(proj1, *first1), preview::invoke(proj2, *first2)))
        return true;
      if (preview::invoke(comp, preview::invoke(proj2, *first2), preview::invoke(proj1, *first1)))
        return false;
    }
    return (first1 == last1) && (first2 != last2);
  }

  template<typename R1, typename R2, typename Proj1 = identity, typename Proj2 = identity, typename Comp = less, std::enable_if_t<
      test_projectable_range<R1, Proj1, R2, Proj2, Comp>
  ::value, int> = 0>
  constexpr bool operator()(R1&& r1, R2&& r2, Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    return (*this)(ranges::begin(r1), ranges::end(r1), ranges::begin(r2), ranges::end(r2), std::ref(comp), std::ref(proj1), std::ref(proj2));
  }

};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::lexicographical_compare_niebloid lexicographical_compare{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_LEXICOGRAPHICAL_COMPARE_H_
