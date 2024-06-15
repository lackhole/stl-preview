//
// Created by yonggyulee on 1/18/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_FIND_H_
#define PREVIEW_ALGORITHM_RANGES_FIND_H_

#include <functional>
#include <type_traits>

#include "preview/__algorithm/ranges/detail/check_input.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/equal_to.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__iterator/indirect_binary_predicate.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

struct find_niebloid {
  template<typename I, typename S, typename T, typename Proj = identity, std::enable_if_t<
      algo_check_binary_input_iterator<indirect_binary_predicate, I, S, const T*, Proj, ranges::equal_to>
  ::value, int> = 0>
  constexpr I operator()(I first, S last, const T& value, Proj proj = {}) const {
    using namespace preview::rel_ops;
    for (; first != last; ++first) {
      if (preview::invoke(proj, *first) == value) {
        return first;
      }
    }
    return first;
  }

  template<typename R, typename T, typename Proj = identity, std::enable_if_t<
      algo_check_binary_input_range<indirect_binary_predicate, R, const T*, Proj, ranges::equal_to>
  ::value, int> = 0>
  constexpr borrowed_iterator_t<R>
  operator()(R&& r, const T& value, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), value, std::ref(proj));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::find_niebloid find{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_FIND_H_
