//
// Created by yonggyulee on 1/18/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_FIND_IF_H_
#define PREVIEW_ALGORITHM_RANGES_FIND_IF_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/indirect_unary_predicate.h"
#include "preview/__iterator/projected.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

struct find_if_niebloid {
  template<typename I, typename S, typename Proj = identity, typename Pred, std::enable_if_t<conjunction<
      input_iterator<I>,
      sentinel_for<S, I>,
      projectable<I, Proj>,
      indirect_unary_predicate<Pred, projected<I, Proj>>
  >::value, int> = 0>
  constexpr I operator()(I first, S last, Pred pred, Proj proj = {}) const {
    using namespace preview::rel_ops;
    for (; first != last; ++first) {
      if (preview::invoke(pred, preview::invoke(proj, *first))) {
        return first;
      }
    }
    return first;
  }

  template<typename R, typename Proj = identity, typename Pred, std::enable_if_t<conjunction<
      input_range<R>,
      projectable<iterator_t<R>, Proj>,
      indirect_unary_predicate<Pred, projected<iterator_t<R>, Proj>>
  >::value, int> = 0>
  constexpr borrowed_iterator_t<R>
  operator()(R&& r, Pred pred, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), preview::wrap_functor(pred), preview::wrap_functor(proj));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::find_if_niebloid find_if{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_FIND_IF_H_
