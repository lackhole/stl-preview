//
// Created by YongGyu Lee on 3/14/24.
//

#ifndef PREVIEW_EXPECTED_UNEXPECT_H_
#define PREVIEW_EXPECTED_UNEXPECT_H_

#include "preview/core.h"

#if PREVIEW_CXX_VERSION >= 23
#include <version>
#endif

#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202211L
#define STD_HAVE_EXPECTED 1
#include <expected>
#else
#define STD_HAVE_EXPECTED 0
#endif

namespace preview {

#if STD_HAVE_EXPECTED

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
