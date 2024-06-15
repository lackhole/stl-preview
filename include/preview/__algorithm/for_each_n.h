//
// Created by YongGyu Lee on 1/25/24.
//

#ifndef PREVIEW_ALGORITHM_FOR_EACH_N_H_
#define PREVIEW_ALGORITHM_FOR_EACH_N_H_

#include <type_traits>

#include "preview/__iterator/iterator_traits/legacy_input_iterator.h"

namespace preview {

template<typename InputIt, typename Size, typename UnaryFunction>
constexpr InputIt for_each_n(InputIt first, Size n, UnaryFunction f) {
  static_assert(LegacyInputIterator<InputIt>::value, "Constraints not satisfied");
  static_assert(std::is_move_constructible<InputIt>::value, "Constraints not satisfied");

  for (Size i = 0; i < n; ++first, (void) ++i) {
    f(*first);
  }

  return first;
}

} // namespace preview

#endif // PREVIEW_ALGORITHM_FOR_EACH_N_H_
