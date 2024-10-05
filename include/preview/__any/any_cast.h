//
// Created by yonggyulee on 2024. 10. 5.
//

#ifndef PREVIEW_ANY_ANY_CAST_H_
#define PREVIEW_ANY_ANY_CAST_H_

#include "preview/core.h"

#if PREVIEW_CXX_VERSION < 17

#include <typeinfo>
#include <type_traits>
#include <utility>

#include "preview/__any/any.h"
#include "preview/__any/bad_any_cast.h"

namespace preview {

template<typename T>
const T* any_cast(const any* operand) noexcept {
  static_assert(!std::is_void<T>::value, "preview::any_cast T must not be void");

  if (operand != nullptr && operand->type() == typeid(T)) {
    return operand->cast<T>();
  }
  return nullptr;
}

template<typename T>
T* any_cast(any* operand) noexcept {
  static_assert(!std::is_void<T>::value, "preview::any_cast T must not be void");

  if (operand != nullptr && operand->type() == typeid(T)) {
    return operand->cast<T>();
  }
  return nullptr;
}

template<typename T>
T any_cast(const any& operand) {
  using U = remove_cvref_t<T>;
  static_assert(std::is_constructible<T, const U&>::value, "preview::any_cast T must be copy constructible from const U&");

  auto ptr = preview::any_cast<U>(&operand);
  if (ptr == nullptr) {
    throw bad_any_cast();
  }
  return static_cast<T>(*ptr);
}

template<typename T>
T any_cast(any& operand) {
  using U = remove_cvref_t<T>;
  static_assert(std::is_constructible<T, U&>::value, "preview::any_cast T must be copy constructible from U&");

  auto ptr = preview::any_cast<U>(&operand);
  if (ptr == nullptr) {
    throw bad_any_cast();
  }
  return static_cast<T>(*ptr);
}

template<typename T>
T any_cast(any&& operand) {
  using U = remove_cvref_t<T>;
  static_assert(std::is_constructible<T, U>::value, "preview::any_cast T must be copy constructible from U");

  auto ptr = preview::any_cast<U>(&operand);
  if (ptr == nullptr) {
    throw bad_any_cast();
  }
  return static_cast<T>(std::move(*ptr));
}

} // namespace preview

#else

#include <any>

namespace preview {

using std::any_cast;

} // namespace preview

#endif

#endif // PREVIEW_ANY_ANY_CAST_H_
