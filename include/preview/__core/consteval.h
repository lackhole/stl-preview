//
// Created by yonggyulee on 2024. 7. 17.
//

#ifndef PREVIEW_CORE_CONSTEVAL_H_
#define PREVIEW_CORE_CONSTEVAL_H_

#include "preview/__core/std_version.h"

#if PREVIEW_CXX_VERSION >= 20
#define PREVIEW_CONSTEVAL consteval
#else
#define PREVIEW_CONSTEVAL constexpr
#endif

#endif // PREVIEW_CORE_CONSTEVAL_H_
