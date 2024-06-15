//
// Created by yonggyulee on 1/25/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_MIN_MAX_RESULT_H_
#define PREVIEW_ALGORITHM_RANGES_MIN_MAX_RESULT_H_

#include <type_traits>
#include <utility>

#include "preview/__core/no_unique_address.h"
#include "preview/__concepts/convertible_to.h"

namespace preview {
namespace ranges {

template<typename T>
struct min_max_result {
  PREVIEW_NO_UNIQUE_ADDRESS T min;
  PREVIEW_NO_UNIQUE_ADDRESS T max;

  template<typename T2, std::enable_if_t<
      convertible_to<const T&, T2>::value, int> = 0>
  constexpr operator min_max_result<T2>() const & {
    return {min, max};
  }

  template<typename T2, std::enable_if_t<
      convertible_to<T, T2>::value, int> = 0>
  constexpr operator min_max_result<T2>() && {
    return {std::move(min), std::move(max)};
  }
};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_MIN_MAX_RESULT_H_
