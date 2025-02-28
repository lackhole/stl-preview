#ifndef PREVIEW_CORE_CONSTEXPR_H_
#define PREVIEW_CORE_CONSTEXPR_H_

#include "preview/__core/cxx_version.h"

#define PREVIEW_CONSTEXPR constexpr

#if PREVIEW_CXX_VERSION >= 14
#define PREVIEW_CONSTEXPR_AFTER_CXX14 PREVIEW_CONSTEXPR
#else
#define PREVIEW_CONSTEXPR_AFTER_CXX14
#endif

#if PREVIEW_CXX_VERSION >= 17
#define PREVIEW_CONSTEXPR_AFTER_CXX17 PREVIEW_CONSTEXPR
#else
#define PREVIEW_CONSTEXPR_AFTER_CXX17
#endif

#if PREVIEW_CXX_VERSION >= 20
#define PREVIEW_CONSTEXPR_AFTER_CXX20 PREVIEW_CONSTEXPR
#else
#define PREVIEW_CONSTEXPR_AFTER_CXX20
#endif

#if PREVIEW_CXX_VERSION >= 23
#define PREVIEW_CONSTEXPR_AFTER_CXX23 PREVIEW_CONSTEXPR
#else
#define PREVIEW_CONSTEXPR_AFTER_CXX23
#endif

#endif // PREVIEW_CORE_CONSTEXPR_H_
