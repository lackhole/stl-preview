//
// Created by yonggyulee on 2024. 9. 29.
//

#ifndef PREVIEW_CORE_MAYBE_UNUSED_H_
#define PREVIEW_CORE_MAYBE_UNUSED_H_

#include "preview/__core/cxx_version.h"

namespace preview {
namespace detail {

template<typename... T>
constexpr void suppress_unused(T&&...) {}

} // namespace detail
} // namespace preview

#if PREVIEW_CXX_VERSION < 17
#  if defined(_MSC_VER)
#    define PREVIEW_MAYBE_UNUSED __pragma(warning(suppress:4100 4101))
#  elif defined(__GNUC__) || defined(__clang__)
#    define MAYBE_UNUSED __attribute__((unused))
#  else
#    define MAYBE_UNUSED
#  endif
#  define PREVIEW_CXX14_SUPPRESS_UNUSED(x) preview::detail::suppress_unused(x)
#else
#  define PREVIEW_MAYBE_UNUSED [[maybe_unused]]
#  define PREVIEW_CXX14_SUPPRESS_UNUSED(x)
#endif

#endif // PREVIEW_CORE_MAYBE_UNUSED_H_
