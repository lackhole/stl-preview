//
// Created by yonggyulee on 1/25/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_IN_FOUND_RESULT_H_
#define PREVIEW_ALGORITHM_RANGES_IN_FOUND_RESULT_H_

#include <type_traits>
#include <utility>

#include "preview/__core/no_unique_address.h"
#include "preview/__concepts/convertible_to.h"

namespace preview {
namespace ranges {

template<typename I>
struct in_found_result {
  PREVIEW_NO_UNIQUE_ADDRESS I in;
  PREVIEW_NO_UNIQUE_ADDRESS bool found;

  template<typename I2, std::enable_if_t<
      convertible_to<const I&, I2>::value, int> = 0>
  constexpr operator in_found_result<I2>() const & {
    return {in, found};
  }

  template<typename I2, std::enable_if_t<
      convertible_to<I, I2>::value, int> = 0>
  constexpr operator in_found_result<I2>() && {
    return {std::move(in), found};
  }
};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_IN_FOUND_RESULT_H_
