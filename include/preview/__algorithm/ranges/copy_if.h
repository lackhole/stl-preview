//
// Created by yonggyulee on 8/13/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_COPY_IF_H_
#define PREVIEW_ALGORITHM_RANGES_COPY_IF_H_

#include <type_traits>
#include <utility>

#include "preview/__algorithm/ranges/in_out_result.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/indirect_unary_predicate.h"
#include "preview/__iterator/indirectly_copyable.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/projected.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__iterator/weakly_incrementable.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {

template<typename I, typename O> using copy_if_result = in_out_result<I, O>;

namespace detail {

struct copy_if_niebloid {
  template<typename I, typename S, typename O, typename Proj = identity, typename Pred, std::enable_if_t<conjunction<
      input_iterator<I>,
      sentinel_for<S, I>,
      weakly_incrementable<O>,
      projectable<I, Proj>,
      indirect_unary_predicate<Pred, projected<I, Proj>>,
      indirectly_copyable<I, O>
  >::value, int> = 0>
  constexpr copy_if_result<I, O> operator()(I first, S last, O result, Pred pred, Proj proj = {}) const {
    while (first != last) {
      if (preview::invoke(pred, preview::invoke(proj, *first))) {
        *result = *first;
        ++result;
      }
      ++first;
    }
    return {std::move(first), std::move(result)};
  }

  template<typename R, typename O, typename Proj = identity, typename Pred, std::enable_if_t<conjunction<
      input_range<R>,
      weakly_incrementable<O>,
      projectable<iterator_t<R>, Proj>,
      indirect_unary_predicate<Pred, projected<iterator_t<R>, Proj>>,
      indirectly_copyable<iterator_t<R>, O>
  >::value, int> = 0>
  constexpr copy_if_result<borrowed_iterator_t<R>, O> operator()(R&& r, O result, Pred pred, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), std::move(result), preview::wrap_functor(pred), preview::wrap_functor(proj));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::copy_if_niebloid copy_if;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_COPY_IF_H_
