//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_CONCEPTS_CONSTRUCTIBLE_FROM_H_
#define PREVIEW_CONCEPTS_CONSTRUCTIBLE_FROM_H_

#include "preview/__concepts/destructible.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {

template<typename T, typename... Args>
struct constructible_from : conjunction<
    destructible<T>,
    std::is_constructible<T, Args...>> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_CONSTRUCTIBLE_FROM_H_
