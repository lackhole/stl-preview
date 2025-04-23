//
// Created by yonggyulee on 14/04/2025
//

#ifndef PREVIEW_ITERATOR_DATA_H_
#define PREVIEW_ITERATOR_DATA_H_

#include <cstddef>
#include <initializer_list>

namespace preview {

template<typename C>
constexpr auto data(C& c) -> decltype(c.data()) {
  return c.data();
}

template<typename C>
constexpr auto data(const C& c) -> decltype(c.data()) {
  return c.data();
}

template<typename T, std::size_t N>
constexpr T* data(T (&array)[N]) noexcept {
  return array;
}

template<typename E>
constexpr const E* data(std::initializer_list<E> il) noexcept {
  return il.begin();
}

} // namespace preview

#endif // PREVIEW_ITERATOR_DATA_H_
