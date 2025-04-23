//
// Created by yonggyulee on 14/04/2025
//

#ifndef PREVIEW_ITERATOR_SSIZE_H_
#define PREVIEW_ITERATOR_SSIZE_H_

#include <cstddef>
#include <type_traits>

#include "preview/__type_traits/common_type.h"

namespace preview {

template<typename C>
constexpr auto ssize(const C& c)
    -> common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(c.size())>>
{
  using R = common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(c.size())>>;
  return static_cast<R>(c.size());
}

template<typename T, std::size_t N>
constexpr std::ptrdiff_t ssize(const T (&)[N]) noexcept
{
  return static_cast<std::ptrdiff_t>(N);
}


} // namespace preview

#endif // PREVIEW_ITERATOR_SSIZE_H_
