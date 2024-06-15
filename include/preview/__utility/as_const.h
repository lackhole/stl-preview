//
// Created by yonggyulee on 2024/01/02.
//

#ifndef PREVIEW_UTILITY_AS_CONST_H_
#define PREVIEW_UTILITY_AS_CONST_H_

#include <type_traits>

namespace preview {

template<typename T>
constexpr std::add_const_t<T>& as_const(T& t) noexcept {
  return t;
}

template<typename T>
void as_const(T&&) = delete;

} // namespace preview

#endif // PREVIEW_UTILITY_AS_CONST_H_
