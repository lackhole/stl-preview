//
// Created by yonggyulee on 2023/12/30.
//

#ifndef PREVIEW_SPAN_DYNAMIC_EXTENT_H_
#define PREVIEW_SPAN_DYNAMIC_EXTENT_H_

#include <cstddef>
#include <limits>

#include "preview/__core/inline_variable.h"

namespace preview {

PREVIEW_INLINE_VARIABLE constexpr std::size_t dynamic_extent = (std::numeric_limits<std::size_t>::max)();

} // namespace preview

#endif // PREVIEW_SPAN_DYNAMIC_EXTENT_H_
