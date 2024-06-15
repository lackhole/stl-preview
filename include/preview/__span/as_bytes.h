//
// Created by yonggyulee on 2023/12/30.
//

#ifndef PREVIEW_SPAN_AS_BYTES_H_
#define PREVIEW_SPAN_AS_BYTES_H_

#include <cstddef>

#include "preview/__core/byte.h"
#include "preview/__span/dynamic_extent.h"
#include "preview/__span/span.h"

namespace preview {
namespace detail {

template<typename T, std::size_t N>
struct as_bytes_size : std::integral_constant<std::size_t, sizeof(T) * N> {};
template<typename T>
struct as_bytes_size<T, dynamic_extent> : std::integral_constant<std::size_t, dynamic_extent> {};

} // namespace detail

template<typename T, std::size_t N>
constexpr span<const byte, detail::as_bytes_size<T, N>::value>
as_bytes(span<T, N> s) noexcept {
  return span<const byte, detail::as_bytes_size<T, N>::value>(
      reinterpret_cast<const byte*>(s.data()),
      s.size_bytes()
  );
}

template<typename T, std::size_t N, std::enable_if_t<std::is_const<T>::value == false, int> = 0>
constexpr span<byte, detail::as_bytes_size<T, N>::value>
as_writable_bytes(span<T, N> s) noexcept {
  return span<byte, detail::as_bytes_size<T, N>::value>(
      reinterpret_cast<byte*>(s.data()),
      s.size_bytes()
  );
}

} // namespace preview

#endif // PREVIEW_SPAN_AS_BYTES_H_
