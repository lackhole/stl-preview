//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_ITERATOR_SENTINEL_FOR_H_
#define PREVIEW_ITERATOR_SENTINEL_FOR_H_

#include "preview/__concepts/semiregular.h"
#include "preview/__concepts/weakly_equality_comparable_with.h"
#include "preview/__iterator/input_or_output_iterator.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {

template<typename S, typename I>
struct sentinel_for
    : conjunction<
        semiregular<S>,
        input_or_output_iterator<I>,
        weakly_equality_comparable_with<S, I>
      > {};

} // namespace preview

#endif // PREVIEW_ITERATOR_SENTINEL_FOR_H_
