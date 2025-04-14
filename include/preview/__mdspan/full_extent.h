//
// Created by yonggyulee on 2024. 9. 27.
//

#ifndef PREVIEW_MDSPAN_FULL_EXTENT_H_
#define PREVIEW_MDSPAN_FULL_EXTENT_H_

#include "preview/__core/inline_variable.h"

namespace preview {

struct full_extent_t {
  explicit full_extent_t() = default;
};

PREVIEW_INLINE_VARIABLE constexpr full_extent_t full_extent{};

} // namespace preview

#endif // PREVIEW_MDSPAN_FULL_EXTENT_H_
