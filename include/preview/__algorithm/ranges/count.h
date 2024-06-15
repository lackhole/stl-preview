//
// Created by yonggyulee on 1/26/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_COUNT_H_
#define PREVIEW_ALGORITHM_RANGES_COUNT_H_

#include <functional>
#include <type_traits>

#include "preview/__algorithm/ranges/detail/check_input.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/equal_to.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__iterator/indirect_binary_predicate.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/range_difference_t.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

struct count_niebloid {
  template<typename I, typename S, typename T, typename Proj = identity, std::enable_if_t<
      algo_check_binary_input_iterator<indirect_binary_predicate, I, S, const T*, Proj, ranges::equal_to>
  ::value, int> = 0>
  constexpr iter_difference_t<I> operator()(I first, S last, const T& value, Proj proj = {}) const {
    using namespace preview::rel_ops;

    iter_difference_t<I> counter = 0;

    for (; first != last; ++first) {
      if (preview::invoke(proj, *first) == value) {
        ++counter;
      }
    }

    return counter;
  }

  template<typename R, typename T, typename Proj = identity, std::enable_if_t<
      algo_check_binary_input_range<indirect_binary_predicate, R, const T*, Proj, ranges::equal_to>::value, int> = 0>
  constexpr range_difference_t<R>
  operator()(R&& r, const T& value, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), value, std::ref(proj));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::count_niebloid count{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_COUNT_H_
