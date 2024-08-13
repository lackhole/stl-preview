//
// Created by yonggyulee on 2024. 8. 10.
//

#ifndef PREVIEW_UTILITY_FORWARD_LIKE_H_
#define PREVIEW_UTILITY_FORWARD_LIKE_H_

#include <type_traits>

#include "preview/__type_traits/copy_cvref.h"

namespace preview {

template<typename T, typename U>
constexpr auto&& forward_like(U&& x) noexcept {
  using CU = copy_const_t<std::remove_reference_t<T>, std::remove_reference_t<U>>;
  using RU = std::conditional_t<std::is_rvalue_reference<T>::value, std::remove_reference_t<CU>&&, CU&>;
  return static_cast<RU>(x);
}

template<typename T, typename U>
using forward_like_t = decltype(preview::forward_like<T>(std::declval<U>()));

} // namespace preview

#endif // PREVIEW_UTILITY_FORWARD_LIKE_H_
