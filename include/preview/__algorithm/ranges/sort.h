//
// Created by YongGyu Lee on 4/15/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_SORT_H_
#define PREVIEW_ALGORITHM_RANGES_SORT_H_

#include <algorithm>
#include <functional>
#include <type_traits>
#include <utility>

#include "preview/__algorithm/ranges/make_heap.h"
#include "preview/__algorithm/ranges/sort_heap.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/less.h"
#include "preview/__iterator/next.h"
#include "preview/__iterator/random_access_iterator.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__iterator/sortable.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/random_access_range.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace detail {

struct sort_niebloid {
  template<typename I, typename S, typename Comp = ranges::less, typename Proj = identity, std::enable_if_t<conjunction<
      random_access_iterator<I>,
      sentinel_for<S, I>,
      sortable<I, Comp, Proj>
  >::value, int> = 0>
  constexpr I operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
    if (first == last)
      return first;

    auto last_iter = ranges::next(first, last);
    ranges::make_heap(first, last_iter, std::ref(comp), std::ref(proj));
    ranges::sort_heap(first, last_iter, std::ref(comp), std::ref(proj));

    return last_iter;
  }

  template<typename R, typename Comp = ranges::less, typename Proj = identity, std::enable_if_t<conjunction<
      random_access_range<R>,
      sortable<iterator_t<R>, Comp, Proj>
  >::value, int> = 0>
  constexpr borrowed_iterator_t<R> operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), std::move(comp), std::move(proj));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::sort_niebloid sort{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_SORT_H_
