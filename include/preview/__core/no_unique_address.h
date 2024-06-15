//
// Created by yonggyulee on 2024/01/04.
//

#ifndef PREVIEW_CORE_NO_UNIQUE_ADDRESS_H_
#define PREVIEW_CORE_NO_UNIQUE_ADDRESS_H_

#include "preview/__core/std_version.h"

#if PREVIEW_CXX_VERSION < 20
#define PREVIEW_NO_UNIQUE_ADDRESS
#else
#define PREVIEW_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#endif // PREVIEW_CORE_NO_UNIQUE_ADDRESS_H_
