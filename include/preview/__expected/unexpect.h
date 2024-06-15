//
// Created by YongGyu Lee on 3/14/24.
//

#ifndef PREVIEW_EXPECTED_UNEXPECT_H_
#define PREVIEW_EXPECTED_UNEXPECT_H_

#if __cplusplus >= 202302L

#include <expected>

#else

#include "preview/__core/inline_variable.h"

#endif

namespace preview {

#if __cplusplus >= 202302L

using unexpect_t = std::unexpect_t;
inline constexpr unexpect_t unexpect{};

#else

struct unexpect_t {
  explicit unexpect_t() = default;
};

PREVIEW_INLINE_VARIABLE constexpr unexpect_t unexpect{};

#endif

} // namespace preview

#endif // PREVIEW_EXPECTED_UNEXPECT_H_
