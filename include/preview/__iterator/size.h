//
// Created by yonggyulee on 14/04/2025
//

#ifndef PREVIEW_ITERATOR_SIZE_H_
#define PREVIEW_ITERATOR_SIZE_H_

#include <cstddef>

namespace preview {

template<typename C>
constexpr auto size(const C& c) -> decltype(c.size())
{
  return c.size();
}

template<typename T, std::size_t N>
constexpr std::size_t size(const T (&)[N]) noexcept
{
  return N;
}

} // namespace preview

#endif // PREVIEW_ITERATOR_SIZE_H_
