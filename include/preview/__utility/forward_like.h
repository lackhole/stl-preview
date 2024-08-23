//
// Created by yonggyulee on 2024. 8. 10.
//

#ifndef PREVIEW_UTILITY_FORWARD_LIKE_H_
#define PREVIEW_UTILITY_FORWARD_LIKE_H_

#include <type_traits>

#include "preview/__type_traits/copy_cvref.h"
#include "preview/__type_traits/override_cvref.h"

namespace preview {

template<typename T, typename U>
constexpr auto&& forward_like(U&& x) noexcept {
  using CU = copy_const_t<std::remove_reference_t<T>, std::remove_reference_t<U>>;
  using RU = override_reference_t<T&&, CU>;
  return static_cast<RU>(x);
}

template<typename T, typename U>
using forward_like_t = decltype(preview::forward_like<T>(std::declval<U>()));

// Same with `forward_like`, but it can remove const qualifier.
template<typename T, typename U>
constexpr auto&& force_forward_like(U&& x) noexcept {
  using CU = override_const_t<std::remove_reference_t<T>, std::remove_reference_t<U>>;
  using RU = override_reference_t<T&&, CU>;
  return static_cast<RU>(const_cast<copy_reference_t<U, CU>>(x));
}

} // namespace preview

#endif // PREVIEW_UTILITY_FORWARD_LIKE_H_
