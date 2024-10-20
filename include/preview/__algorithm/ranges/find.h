//
// Created by yonggyulee on 1/18/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_FIND_H_
#define PREVIEW_ALGORITHM_RANGES_FIND_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__functional/equal_to.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/indirect_binary_predicate.h"
#include "preview/__iterator/projected.h"
#include "preview/__iterator/projected_value_t.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

struct find_niebloid {
  template<typename I, typename S, typename Proj = identity, typename T = projected_value_t<I, S>, std::enable_if_t<conjunction<
      input_iterator<I>,
      sentinel_for<S, I>,
      projectable<I, Proj>,
      indirect_binary_predicate<ranges::equal_to, projected<I, Proj>, const T*>
  >::value, int> = 0>
  constexpr I operator()(I first, S last, const T& value, Proj proj = {}) const {
    using namespace preview::rel_ops;
    for (; first != last; ++first) {
      if (preview::invoke(proj, *first) == value) {
        return first;
      }
    }
    return first;
  }

  template<typename R, typename Proj = identity, typename T = projected_value_t<iterator_t<R>, Proj>, std::enable_if_t<conjunction<
      input_range<R>,
      projectable<iterator_t<R>, Proj>,
      indirect_binary_predicate<ranges::equal_to, projected<iterator_t<R>, Proj>, const T*>
  >::value, int> = 0>
  constexpr borrowed_iterator_t<R>
  operator()(R&& r, const T& value, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), value, preview::wrap_functor(proj));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::find_niebloid find{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_FIND_H_
