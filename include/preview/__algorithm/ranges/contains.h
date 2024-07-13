//
// Created by YongGyu Lee on 4/15/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_CONTAINS_H_
#define PREVIEW_ALGORITHM_RANGES_CONTAINS_H_

#include <type_traits>
#include <utility>

#include "preview/__algorithm/ranges/find.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/equal_to.h"
#include "preview/__functional/identity.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/indirectly_comparable.h"
#include "preview/__iterator/indirect_binary_predicate.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/projected.h"
#include "preview/__iterator/projected_value_t.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {

namespace detail {

struct contains_niebloid {
  template<typename I, typename S, typename Proj = identity, typename T = projected_value_t<I, Proj>, std::enable_if_t<conjunction<
      input_iterator<I>,
      sentinel_for<S, I>,
      indirect_binary_predicate<ranges::equal_to, projected<I, Proj>, const T*>
  >::value, int> = 0>
  constexpr bool operator()(I first, S last, const T& value, Proj proj = {}) const {
    using namespace rel_ops;
    return ranges::find(std::move(first), last, value, proj) != last;
  }

  template<typename R, typename Proj = identity, typename T = projected_value_t<iterator_t<R>, Proj>, std::enable_if_t<conjunction<
      input_range<R>,
      projectable<iterator_t<R>, Proj>,
      indirect_binary_predicate<ranges::equal_to, projected<iterator_t<R>, Proj>, const T*>
  >::value, int> = 0>
  constexpr bool operator()(R&& r, const T& value, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), std::move(value), proj);
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::contains_niebloid contains{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_CONTAINS_H_
