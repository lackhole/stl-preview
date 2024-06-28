//
// Created by YongGyu Lee on 2024. 6. 28
//

#ifndef PREVIEW_TYPE_TRAITS_TO_UNSIGNED_LIKE_H_
#define PREVIEW_TYPE_TRAITS_TO_UNSIGNED_LIKE_H_

#include <type_traits>
#include "preview/__type_traits/is_integer_like.h"

namespace preview {

template<typename T, std::enable_if_t<is_integer_like<T>::value, int> = 0>
constexpr auto to_unsigned_like(T x) noexcept {
  return static_cast<std::make_unsigned_t<T>>(x);
}

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_TO_UNSIGNED_LIKE_H_
