//
// Created by yonggyulee on 2023/12/25.
//

#ifndef PREVIEW_CONCEPTS_SIGNED_INTEGRAL_H_
#define PREVIEW_CONCEPTS_SIGNED_INTEGRAL_H_

#include <type_traits>

#include "preview/__type_traits/conjunction.h"

namespace preview {

template<typename T>
struct signed_integral : conjunction<std::is_integral<T>, std::is_signed<T>> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_SIGNED_INTEGRAL_H_
