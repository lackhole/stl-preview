//
// Created by yonggyulee on 14/04/2025
//

#ifndef PREVIEW_ITERATOR_EMPTY_H_
#define PREVIEW_ITERATOR_EMPTY_H_

#include <cstddef>
#include <initializer_list>

namespace preview {

template<typename C>
constexpr auto empty(const C& c) -> decltype(c.empty()) {
  return c.empty();
}

template<typename T, std::size_t N>
constexpr bool empty(const T (&)[N]) noexcept {
  return false;
}

template<typename E>
constexpr bool empty(std::initializer_list<E>& il) noexcept {
  return il.size() == 0;
}

} // namespace preview

#endif // PREVIEW_ITERATOR_EMPTY_H_
