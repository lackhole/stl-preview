//
// Created by yonggyulee on 2024. 8. 4.
//

#ifndef PREVIEW_MEMORY_POINTER_CAST_H_
#define PREVIEW_MEMORY_POINTER_CAST_H_

#include <memory>
#include <utility>

namespace preview {
// Also implement C++11 versions to provide same call syntax

template<typename T, typename U>
std::shared_ptr<T> static_pointer_cast(const std::shared_ptr<U>& r) noexcept {
  const auto ptr = static_cast<typename std::shared_ptr<T>::element_type*>(r.get());
  return std::shared_ptr<T>(r, ptr);
}

template<typename T, typename U>
std::shared_ptr<T> static_pointer_cast(std::shared_ptr<U>&& r) noexcept {
  const auto ptr = static_cast<typename std::shared_ptr<T>::element_type*>(r.get());
  std::shared_ptr<T> aliased_ptr(std::move(r), ptr);
  r.reset();
  return aliased_ptr;
}

template<typename T, typename U>
std::shared_ptr<T> dynamic_pointer_cast(const std::shared_ptr<U>& r) noexcept {
  const auto ptr = dynamic_cast<typename std::shared_ptr<T>::element_type*>(r.get());
  if (ptr == nullptr)
    return {};

  return std::shared_ptr<T>(r, ptr);
}

template<typename T, typename U>
std::shared_ptr<T> dynamic_pointer_cast(std::shared_ptr<U>&& r) noexcept {
  const auto ptr = dynamic_cast<typename std::shared_ptr<T>::element_type*>(r.get());
  if (ptr == nullptr)
    return {};

  std::shared_ptr<T> aliased_ptr(std::move(r), ptr);
  r.reset();
  return aliased_ptr;
}

template<typename T, typename U>
std::shared_ptr<T> const_pointer_cast(const std::shared_ptr<U>& r) noexcept {
  const auto ptr = const_cast<typename std::shared_ptr<T>::element_type*>(r.get());
  return std::shared_ptr<T>(r, ptr);
}

template<typename T, typename U>
std::shared_ptr<T> const_pointer_cast(std::shared_ptr<U>&& r) noexcept {
  const auto ptr = const_cast<typename std::shared_ptr<T>::element_type*>(r.get());
  std::shared_ptr<T> aliased_ptr(std::move(r), ptr);
  r.reset();
  return aliased_ptr;
}

template<typename T, typename U>
std::shared_ptr<T> reinterpret_pointer_cast(const std::shared_ptr<U>& r) noexcept {
  const auto ptr = reinterpret_cast<typename std::shared_ptr<T>::element_type*>(r.get());
  return std::shared_ptr<T>(r, ptr);
}

template<typename T, typename U>
std::shared_ptr<T> reinterpret_pointer_cast(std::shared_ptr<U>&& r) noexcept {
  const auto ptr = reinterpret_cast<typename std::shared_ptr<T>::element_type*>(r.get());
  std::shared_ptr<T> aliased_ptr(std::move(r), ptr);
  r.reset();
  return aliased_ptr;
}

} // namespace preview

#endif // PREVIEW_MEMORY_POINTER_CAST_H_
