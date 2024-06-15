//
// Created by yonggyulee on 1/25/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_IN_VALUE_RESULT_H_
#define PREVIEW_ALGORITHM_RANGES_IN_VALUE_RESULT_H_

#include <type_traits>
#include <utility>

#include "preview/__core/no_unique_address.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {

template<typename I, typename T>
struct in_value_result {
  PREVIEW_NO_UNIQUE_ADDRESS I in;
  PREVIEW_NO_UNIQUE_ADDRESS T value;

  template<typename I2, typename T2, std::enable_if_t<conjunction<
      convertible_to<const I&, I2>,
      convertible_to<const T&, T2>
  >::value, int> = 0>
  constexpr operator in_value_result<I2, T2>() const & {
    return {in, value};
  }

  template<typename I2, typename T2, std::enable_if_t<conjunction<
      convertible_to<I, I2>,
      convertible_to<T, T2>
  >::value, int> = 0>
  constexpr operator in_value_result<I2, T2>() && {
    return {std::move(in), std::move(value)};
  }
};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_IN_VALUE_RESULT_H_
