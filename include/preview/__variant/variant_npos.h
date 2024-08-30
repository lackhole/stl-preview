//
// Created by YongGyu Lee on 11/3/23.
//

#ifndef PREVIEW_VARIANT_VARIANT_NPOS_H_
#define PREVIEW_VARIANT_VARIANT_NPOS_H_

#include <cstddef>

#include "preview/__core/inline_variable.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#endif

namespace preview {

PREVIEW_INLINE_VARIABLE constexpr std::size_t variant_npos = -1;

} // namespace preview

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // PREVIEW_VARIANT_VARIANT_NPOS_H_
