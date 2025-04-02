//
// Created by yonggyulee on 02/04/2025
//

#ifndef PREVIEW_CORE_STATIC_ASSERT_H_
#define PREVIEW_CORE_STATIC_ASSERT_H_

#include "preview/__core/cxx_version.h"

#if PREVIEW_CXX_VERSION >= 17
#define PREVIEW_STATIC_ASSERT(...) static_assert(__VA_ARGS__)
#else
#define PREVIEW_STATIC_ASSERT(...) static_assert(__VA_ARGS__, "")
#endif

#endif // PREVIEW_CORE_STATIC_ASSERT_H_
