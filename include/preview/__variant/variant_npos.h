//
// Created by YongGyu Lee on 11/3/23.
//

#ifndef PREVIEW_VARIANT_VARIANT_NPOS_H_
#define PREVIEW_VARIANT_VARIANT_NPOS_H_

#include <cstddef>

#include "preview/__core/inline_variable.h"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4245) // conversion from 'size_t' to 'int', possible loss of data
#endif

namespace preview {

PREVIEW_INLINE_VARIABLE constexpr std::size_t variant_npos = -1;

} // namespace preview

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif // PREVIEW_VARIANT_VARIANT_NPOS_H_
