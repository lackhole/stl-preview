//
// Created by yonggyulee on 1/26/24.
//

#ifndef PREVIEW_ITERATOR_PERMUTABLE_H_
#define PREVIEW_ITERATOR_PERMUTABLE_H_

#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/indirectly_movable_storable.h"
#include "preview/__iterator/indirectly_swappable.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {

template<typename I>
struct permutable
    : conjunction<
          forward_iterator<I>,
          indirectly_movable_storable<I, I>,
          indirectly_swappable<I, I>
      > {};

} // namespace preview

#endif // PREVIEW_ITERATOR_PERMUTABLE_H_
