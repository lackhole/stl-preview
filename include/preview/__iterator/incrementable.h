//
// Created by yonggyulee on 2023/12/25.
//

#ifndef PREVIEW_ITERATOR_INCREMENTABLE_H_
#define PREVIEW_ITERATOR_INCREMENTABLE_H_

#include "preview/__concepts/regular.h"
#include "preview/__iterator/weakly_incrementable.h"

namespace preview {

template<typename T>
struct incrementable
    : conjunction<
        regular<T>,
        weakly_incrementable<T>
      > {};

} // namespace preview

#endif // PREVIEW_ITERATOR_INCREMENTABLE_H_
