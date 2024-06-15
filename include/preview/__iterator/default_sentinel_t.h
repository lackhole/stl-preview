//
// Created by cosge on 2024-01-03.
//

#ifndef PREVIEW_ITERATOR_DEFAULT_SENTINEL_T_H_
#define PREVIEW_ITERATOR_DEFAULT_SENTINEL_T_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"

namespace preview {

struct default_sentinel_t {};

PREVIEW_INLINE_VARIABLE constexpr default_sentinel_t default_sentinel{};

} // namespace preview

#endif // PREVIEW_ITERATOR_DEFAULT_SENTINEL_T_H_
