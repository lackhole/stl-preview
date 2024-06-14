#ifndef PREVIEW_CORE_INLINE_VARIABLE_H_
#define PREVIEW_CORE_INLINE_VARIABLE_H_

#include "preview/__core/std_version.h"

#if PREVIEW_CXX_VERSION < 17
#define PREVIEW_INLINE_VARIABLE static
#else
#define PREVIEW_INLINE_VARIABLE inline
#endif

#endif // PREVIEW_CORE_INLINE_VARIABLE_H_
