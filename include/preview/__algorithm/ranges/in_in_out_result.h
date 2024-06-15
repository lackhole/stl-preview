//
// Created by yonggyulee on 1/25/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_IN_IN_OUT_RESULT_H_
#define PREVIEW_ALGORITHM_RANGES_IN_IN_OUT_RESULT_H_

#include <type_traits>
#include <utility>

#include "preview/__core/no_unique_address.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {

template<typename I1, typename I2, typename O>
struct in_in_out_result {
  PREVIEW_NO_UNIQUE_ADDRESS I1 in1;
  PREVIEW_NO_UNIQUE_ADDRESS I2 in2;
  PREVIEW_NO_UNIQUE_ADDRESS O out;

  template<typename II1, typename II2, typename OO, std::enable_if_t<conjunction<
      convertible_to<const I1&, II1>,
      convertible_to<const I2&, II2>,
      convertible_to<const O&, OO>
  >::value, int> = 0>
  constexpr operator in_in_out_result<I2, II2, OO>() const & {
    return {in1, in2, out};
  }

  template<typename II1, typename II2, typename OO, std::enable_if_t<conjunction<
      convertible_to<I1, II1>,
      convertible_to<I2, II2>,
      convertible_to<O, OO>
  >::value, int> = 0>
  constexpr operator in_in_out_result<I2, II2, OO>() && {
    return {std::move(in1), std::move(in2), std::move(out)};
  }
};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_IN_IN_OUT_RESULT_H_
