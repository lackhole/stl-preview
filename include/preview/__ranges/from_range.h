//
// Created by yonggyulee on 2024/01/06.
//

#ifndef PREVIEW_RANGES_FROM_RANGE_H_
#define PREVIEW_RANGES_FROM_RANGE_H_

#if __cplusplus >= 202302L
#include <ranges>
#endif

#include "preview/__core/inline_variable.h"


namespace preview {
namespace ranges {

#if __cplusplus >= 202302L
using from_range_t = std::ranges::from_range_t;
#else
struct from_range_t {
  explicit from_range_t() = default;
};
#endif

PREVIEW_INLINE_VARIABLE constexpr from_range_t from_range{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_FROM_RANGE_H_
