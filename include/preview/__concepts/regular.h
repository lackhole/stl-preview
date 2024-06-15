//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_CONCEPTS_REGULAR_H_
#define PREVIEW_CONCEPTS_REGULAR_H_

#include "preview/__concepts/semiregular.h"
#include "preview/__concepts/equality_comparable.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {

template<typename T>
struct regular
    : conjunction<
        semiregular<T>,
        equality_comparable<T>
      > {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_REGULAR_H_
