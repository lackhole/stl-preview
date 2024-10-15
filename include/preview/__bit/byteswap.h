//
// Created by Chi-Iroh on 03/10/2024.
//

#ifndef PREVIEW_BIT_BYTESWAP_H_
#define PREVIEW_BIT_BYTESWAP_H_

#include <climits>
#include <cstdint>
#include <type_traits>

#include "preview/config.h"
#include "preview/__bit/bit_cast.h"

#if PREVIEW_CXX_VERSION >= 23
  #include <bit>
#endif

namespace preview {
namespace detail {

template<typename T>
constexpr T byteswap_impl(T n, std::true_type /* is unsigned */) noexcept {
  constexpr T byte_mask{ (1 << CHAR_BIT) - 1 };
  T result{ 0 };
  unsigned upper_shift{ CHAR_BIT * (sizeof(T) - 1) };
  unsigned lower_shift{ 0 };

  for (unsigned i{ 0 }; i < sizeof(T); i++) {
    const T byte{ static_cast<T>((n & (byte_mask << upper_shift)) >> upper_shift) };
    result |= byte << lower_shift;
    lower_shift += CHAR_BIT;
    upper_shift -= CHAR_BIT;
  }
  return result;
}

template<typename T>
constexpr T byteswap_impl(T n, std::false_type /* is signed */) {
  using UnsignedT = typename std::make_unsigned<T>::type;
  return preview::bit_cast<T>(preview::detail::byteswap_impl<UnsignedT>(preview::bit_cast<UnsignedT>(n), std::true_type{}));
}

template<typename T, std::size_t Size>
constexpr inline T byteswap_check_size(T n, std::integral_constant<std::size_t, Size>) {
  return preview::detail::byteswap_impl<T>(n, std::is_unsigned<T>{});
}

template<typename T>
constexpr inline T byteswap_check_size(T n, std::integral_constant<std::size_t, 1>) {
  return n;
}

} // namespace detail

template<typename T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
constexpr inline T byteswap(T n) noexcept {
#if PREVIEW_CXX_VERSION >= 23
  return std::byteswap<T>(n);
#else
  return preview::detail::byteswap_check_size<T>(n, std::integral_constant<std::size_t, sizeof(T)>{});
#endif
}

} // namespace preview

#endif