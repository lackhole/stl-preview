//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_CONCEPTS_MOVE_CONSTRUCTIBLE_H_
#define PREVIEW_CONCEPTS_MOVE_CONSTRUCTIBLE_H_

#include "preview/__concepts/constructible_from.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {

template<typename T>
struct move_constructible : conjunction<constructible_from<T, T>, convertible_to<T, T>> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_MOVE_CONSTRUCTIBLE_H_
