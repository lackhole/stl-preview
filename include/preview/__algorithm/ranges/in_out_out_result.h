//
// Created by yonggyulee on 1/25/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_IN_OUT_OUT_RESULT_H_
#define PREVIEW_ALGORITHM_RANGES_IN_OUT_OUT_RESULT_H_

#include <type_traits>
#include <utility>

#include "preview/__core/no_unique_address.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {

template<typename I, typename O1, typename O2>
struct in_out_out_result {
  PREVIEW_NO_UNIQUE_ADDRESS I in;
  PREVIEW_NO_UNIQUE_ADDRESS O1 out1;
  PREVIEW_NO_UNIQUE_ADDRESS O2 out2;

  template<typename II, typename OO1, typename OO2, std::enable_if_t<conjunction<
      convertible_to<const I&, II>,
      convertible_to<const O1&, OO1>,
      convertible_to<const O2&, OO2>
  >::value, int> = 0>
  constexpr operator in_out_out_result<II, OO1, OO2>() const & {
    return {in, out1, out2};
  }

  template<typename II, typename OO1, typename OO2, std::enable_if_t<conjunction<
      convertible_to<I, II>,
      convertible_to<O1, OO1>,
      convertible_to<O2, OO2>
  >::value, int> = 0>
  constexpr operator in_out_out_result<II, OO1, OO2>() && {
    return {std::move(in), std::move(out1), std::move(out2)};
  }
};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_IN_OUT_OUT_RESULT_H_
