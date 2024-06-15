//
// Created by yonggyulee on 2024/01/06.
//

#ifndef PREVIEW_RANGES_FROM_RANGE_H_
#define PREVIEW_RANGES_FROM_RANGE_H_

#include "preview/core.h"

#if PREVIEW_CXX_VERSION >= 23
#include <ranges>
#include <version>
#endif

#include "preview/__core/inline_variable.h"

namespace preview {

#if defined(__cpp_lib_containers_ranges) && __cpp_lib_containers_ranges >= 202202L
using from_range_t = std::from_range_t;
#else
struct from_range_t {
  explicit from_range_t() = default;
};
#endif

PREVIEW_INLINE_VARIABLE constexpr from_range_t from_range{};

} // namespace preview

#endif // PREVIEW_RANGES_FROM_RANGE_H_
