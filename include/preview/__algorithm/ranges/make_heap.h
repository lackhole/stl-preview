//
// Created by YongGyu Lee on 4/15/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_MAKE_HEAP_H_
#define PREVIEW_ALGORITHM_RANGES_MAKE_HEAP_H_

#include <algorithm>
#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/less.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/next.h"
#include "preview/__iterator/random_access_iterator.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__iterator/sortable.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"

namespace preview {
namespace ranges {
namespace detail {

struct make_heap_niebloid {
 public:
  template<typename I, typename S, typename Comp = ranges::less, typename Proj = identity, std::enable_if_t<conjunction<
      random_access_iterator<I>,
      sentinel_for<S, I>,
      sortable<I, Comp, Proj>
  >::value, int> = 0>
  constexpr I operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
    auto last_iter = ranges::next(first, last);

    std::make_heap(std::move(first), last_iter, [&comp, &proj](auto&& lhs, auto&& rhs) {
      return preview::invoke(
          comp,
          preview::invoke(proj, std::forward<decltype(lhs)>(lhs)),
          preview::invoke(proj, std::forward<decltype(rhs)>(rhs))
      );
    });
    return last_iter;
  }

  template<typename R, typename Comp = ranges::less, typename Proj = identity, std::enable_if_t<conjunction<
      random_access_range<R>,
      sortable<iterator_t<R>, Comp, Proj>
  >::value, int> = 0>
  constexpr iterator_t<R> operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), preview::wrap_functor(comp), preview::wrap_functor(proj));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::make_heap_niebloid make_heap{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_MAKE_HEAP_H_
