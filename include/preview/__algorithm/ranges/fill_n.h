//
// Created by yonggyulee on 2024. 8. 24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_FILL_N_H_
#define PREVIEW_ALGORITHM_RANGES_FILL_N_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__iterator/output_iterator.h"

namespace preview {
namespace ranges {
namespace detail {

struct fill_n_niebloid {
  template<typename O, typename T = iter_value_t<O>, std::enable_if_t<
      output_iterator<O, const T&>
  ::value, int> = 0>
  constexpr O operator()(O first, iter_difference_t<O> n, const T& value) const {
    while (n != 0) {
      *first = value;
      ++first;
      --n;
    }
    return first;
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::fill_n_niebloid fill_n{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_FILL_N_H_
