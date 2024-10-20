//
// Created by YongGyu Lee on 01/08/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_MAX_ELEMENT_H_
#define PREVIEW_ALGORITHM_RANGES_MAX_ELEMENT_H_

#include <functional>
#include <type_traits>

#include "preview/__algorithm/ranges/detail/check_input.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/less.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/indirect_strict_weak_order.h"
#include "preview/__iterator/projected.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace detail {

struct max_element_niebloid {
  template<typename I, typename S, typename Proj = identity, typename Comp = ranges::less, std::enable_if_t<
      algo_check_unary_forward_iterator<indirect_strict_weak_order, I, S, Proj, Comp>
  ::value, int> = 0>
  constexpr I operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
    if (first == last)
      return last;

    auto it = first;
    while (++first != last) {
      if (preview::invoke(comp, preview::invoke(proj, *it), preview::invoke(proj, *first)))
        it = first;
    }
    return it;
  }

  template<typename R, typename Proj = identity, typename Comp = less, std::enable_if_t<
      algo_check_unary_forward_range<indirect_strict_weak_order, R, Proj, Comp>
  ::value, int> = 0>
  constexpr borrowed_iterator_t<R>
  operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), preview::wrap_functor(comp), preview::wrap_functor(proj));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::max_element_niebloid max_element{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_MAX_ELEMENT_H_
