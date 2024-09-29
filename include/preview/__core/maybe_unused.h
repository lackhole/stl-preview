//
// Created by yonggyulee on 2024. 9. 29.
//

#ifndef PREVIEW_CORE_MAYBE_UNUSED_H_
#define PREVIEW_CORE_MAYBE_UNUSED_H_

#include "preview/__core/std_version.h"

namespace preview {
namespace detail {

template<typename... T>
constexpr void suppress_unused(T&&...) {}

} // namespace detail
} // namespace preview

#if PREVIEW_CXX_VERSION < 17
#  define PREVIEW_MAYBE_UNUSED
#  define PREVIEW_CXX14_SUPPRESS_UNUSED(x) preview::detail::suppress_unused(x)
#else
#  define PREVIEW_MAYBE_UNUSED [[maybe_unused]]
#  define PREVIEW_CXX14_SUPPRESS_UNUSED(x)
#endif

#endif // PREVIEW_CORE_MAYBE_UNUSED_H_
