//
// Created by yonggyulee on 1/12/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_IN_OUT_RESULT_H_
#define PREVIEW_ALGORITHM_RANGES_IN_OUT_RESULT_H_

#include <type_traits>
#include <utility>

#include "preview/__core/no_unique_address.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {

template<typename I, typename O>
struct in_out_result {
  PREVIEW_NO_UNIQUE_ADDRESS I in;
  PREVIEW_NO_UNIQUE_ADDRESS O out;

  template<typename I2, typename O2, std::enable_if_t<conjunction<
      convertible_to<const I&, I2>,
      convertible_to<const O&, O2>
  >::value, int> = 0>
  constexpr operator in_out_result<I2, O2>() const & {
    return {in, out};
  }

  template<typename I2, typename O2, std::enable_if_t<conjunction<
      convertible_to<I, I2>,
      convertible_to<O, O2>
  >::value, int> = 0>
  constexpr operator in_out_result<I2, O2>() && {
    return {std::move(in), std::move(out)};
  }
};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_IN_OUT_RESULT_H_
