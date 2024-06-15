//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_VIEW_H_
#define PREVIEW_RANGES_VIEW_H_

#include "preview/__concepts/movable.h"
#include "preview/__ranges/enable_view.h"
#include "preview/__ranges/range.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {

template<typename T>
struct view
    : conjunction<
          range<T>,
          movable<T>,
          enable_view<T>
      > {};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEW_H_
