//
// Created by yonggyulee on 2024. 7. 14..
//

#ifndef PREVIEW_CONCEPTS_INTEGRAL_H_
#define PREVIEW_CONCEPTS_INTEGRAL_H_

#include <type_traits>

#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/negation.h"

namespace preview {

template<typename T>
struct integral : std::is_integral<T> {};

template<typename T>
struct signed_integral : conjunction<integral<T>, std::is_signed<T>> {};

template<typename T>
struct unsigned_integral : conjunction<integral<T>, negation<std::is_signed<T>>> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_INTEGRAL_H_
