//
// Created by yonggyulee on 1/28/24.
//

#ifndef PREVIEW_MEMORY_ADDRESSOF_H_
#define PREVIEW_MEMORY_ADDRESSOF_H_

#include <memory>
#include <type_traits>

namespace preview {

# if __cplusplus < 201703L
#  ifdef PREVIEW_CONSTEXPR_BUILTIN_ADDRESSOF_CXX14
template<typename T>
constexpr T* addressof(T& t) noexcept {
  return __builtin_addressof(t);
}
#  else
template<typename T>
std::enable_if_t<std::is_object<T>::value, T*>
addressof(T& t) noexcept {
  return
    reinterpret_cast<T*>(
      &const_cast<char&>(
        reinterpret_cast<const volatile char&>(t)
      )
    );
}
template<typename T>
constexpr std::enable_if_t<std::is_object<T>::value == false, T*>
addressof(T& t) noexcept {
  return &t;
}
#  endif
# else
template<typename T>
constexpr T* addressof(T& t) noexcept {
  return std::addressof(t);
}
# endif

template<typename T>
constexpr const T* addressof(T&&) = delete;

} // namespace preview

#endif // PREVIEW_MEMORY_ADDRESSOF_H_
