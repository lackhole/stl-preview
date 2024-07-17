//
// Created by YongGyu Lee on 2024. 6. 28
//

#ifndef PREVIEW_UTILITY_TO_UNSIGNED_LIKE_H_
#define PREVIEW_UTILITY_TO_UNSIGNED_LIKE_H_

#include <type_traits>

#include "preview/__concepts/integer_like.h"
#include "preview/__type_traits/make_unsigned_like.h"

namespace preview {

template<typename T, std::enable_if_t<integer_like<T>::value, int> = 0>
constexpr make_unsigned_like_t<T> to_unsigned_like(T x) noexcept {
  return static_cast<make_unsigned_like_t<T>>(x);
}

} // namespace preview

#endif // PREVIEW_UTILITY_TO_UNSIGNED_LIKE_H_
