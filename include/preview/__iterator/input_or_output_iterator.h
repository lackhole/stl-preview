//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_ITERATOR_INPUT_OR_OUTPUT_ITERATOR_H_
#define PREVIEW_ITERATOR_INPUT_OR_OUTPUT_ITERATOR_H_

#include "preview/__concepts/dereferenceable.h"
#include "preview/__iterator/weakly_incrementable.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {

template<typename I>
struct input_or_output_iterator
    : conjunction<
        dereferenceable<I>,
        weakly_incrementable<I>
      > {};

} // namespace preview

#endif // PREVIEW_ITERATOR_INPUT_OR_OUTPUT_ITERATOR_H_
