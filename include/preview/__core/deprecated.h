//
// Created by yonggyulee on 2024. 11. 3.
//

#ifndef PREVIEW_CORE_DEPRECATED_H_
#define PREVIEW_CORE_DEPRECATED_H_

#include "preview/__core/cxx_version.h"

#define PREVIEW_DEPRECATED(message) [[deprecated(message)]]

#define PREVIEW_DEPRECATED_SINCE_CXX14(message) PREVIEW_DEPRECATED(message)
#define PREVIEW_DEPRECATED_SINCE_CXX17(message) PREVIEW_DEPRECATED(message)
#define PREVIEW_DEPRECATED_SINCE_CXX20(message) PREVIEW_DEPRECATED(message)
#define PREVIEW_DEPRECATED_SINCE_CXX23(message) PREVIEW_DEPRECATED(message)
#define PREVIEW_DEPRECATED_SINCE_CXX26(message) PREVIEW_DEPRECATED(message)

#if PREVIEW_CXX_VERSION < 14
#undef PREVIEW_DEPRECATED_SINCE_CXX14
#define PREVIEW_DEPRECATED_SINCE_CXX14(message)
#endif

#if PREVIEW_CXX_VERSION < 17
#undef PREVIEW_DEPRECATED_SINCE_CXX17
#define PREVIEW_DEPRECATED_SINCE_CXX17(message)
#endif

#if PREVIEW_CXX_VERSION < 20
#undef PREVIEW_DEPRECATED_SINCE_CXX20
#define PREVIEW_DEPRECATED_SINCE_CXX20(message)
#endif

#if PREVIEW_CXX_VERSION < 23
#undef PREVIEW_DEPRECATED_SINCE_CXX23
#define PREVIEW_DEPRECATED_SINCE_CXX23(message)
#endif

#if PREVIEW_CXX_VERSION < 26
#undef PREVIEW_DEPRECATED_SINCE_CXX26
#define PREVIEW_DEPRECATED_SINCE_CXX26(message)
#endif

#endif // PREVIEW_CORE_DEPRECATED_H_
