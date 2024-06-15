//
// Created by yonggyulee on 1/10/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_IN_FUN_RESULT_H_
#define PREVIEW_ALGORITHM_RANGES_IN_FUN_RESULT_H_

#include <type_traits>
#include <utility>

#include "preview/__core/no_unique_address.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {

template<typename I, typename F>
struct in_fun_result {
  PREVIEW_NO_UNIQUE_ADDRESS I in;
  PREVIEW_NO_UNIQUE_ADDRESS F fun;

  template<typename I2, typename F2, std::enable_if_t<conjunction<
      convertible_to<const I&, I2>,
      convertible_to<const F&, F2>
  >::value, int> = 0>
  constexpr operator in_fun_result<I2, F2>() const & {
    return {in, fun};
  }

  template<typename I2, typename F2, std::enable_if_t<conjunction<
      convertible_to<const I&, I2>,
      convertible_to<const F&, F2>
  >::value, int> = 0>
  constexpr operator in_fun_result<I2, F2>() && {
    return {std::move(in), std::move(fun)};
  }
};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_IN_FUN_RESULT_H_
