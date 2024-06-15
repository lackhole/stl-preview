//
// Created by yonggyulee on 2023/12/27.
//

#ifndef PREVIEW_CONCEPTS_RELATION_H_
#define PREVIEW_CONCEPTS_RELATION_H_

#include "preview/__concepts/predicate.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {

template<typename R, typename T, typename U>
struct relation
    : conjunction<
        predicate<R, T, T>,
        predicate<R, U, U>,
        predicate<R, T, U>,
        predicate<R, U, T>
      > {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_RELATION_H_
