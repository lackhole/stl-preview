//
// Created by yonggyulee on 2023/12/27.
//

#ifndef PREVIEW_CONCEPTS_UNSIGNED_INTEGRAL_H_
#define PREVIEW_CONCEPTS_UNSIGNED_INTEGRAL_H_

#include <type_traits>

#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/negation.h"

namespace preview {

template<typename T>
struct unsigned_integral : conjunction<std::is_integral<T>, negation<std::is_signed<T>>> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_UNSIGNED_INTEGRAL_H_
