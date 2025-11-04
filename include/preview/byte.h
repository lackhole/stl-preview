#ifndef PREVIEW_CORE_BYTE_H_
#define PREVIEW_CORE_BYTE_H_

#include <cstddef>
#include <type_traits>

#include "preview/__core/cxx_version.h"

namespace preview {

#if PREVIEW_CXX_VERSION < 17

class byte {
 public:
  byte() noexcept = default;

  constexpr explicit byte(unsigned char c) noexcept : value_(c) {}

  constexpr explicit operator unsigned char() const noexcept {
    return value_;
  }

  friend constexpr bool operator==(byte x, byte y) noexcept {
    return x.value_ == y.value_;
  }

  friend constexpr bool operator!=(byte x, byte y) noexcept {
    return x.value_ != y.value_;
  }

 private:
  unsigned char value_;
};

template<typename IntegerType>
constexpr std::enable_if_t<std::is_integral<IntegerType>::value, IntegerType>
to_integer(byte b) noexcept {
  return IntegerType(b.operator unsigned char());
}

template <class IntegerType>
constexpr std::enable_if_t<std::is_integral<IntegerType>::value, byte>
operator<<(byte b, IntegerType shift) noexcept {
  return byte{
    static_cast<unsigned char>(
      static_cast<unsigned int>(b.operator unsigned char()) << shift
    )
  };
}

template <class IntegerType>
constexpr std::enable_if_t<std::is_integral<IntegerType>::value, byte>
operator>>(byte b, IntegerType shift) noexcept {
  return byte{
    static_cast<unsigned char>(
      static_cast<unsigned int>(b.operator unsigned char()) >> shift
    )
  };
}

template <class IntegerType>
constexpr std::enable_if_t<std::is_integral<IntegerType>::value, byte&>
operator<<=(byte& b, IntegerType shift) noexcept {
  return b = b << shift;
}

template <class IntegerType>
constexpr std::enable_if_t<std::is_integral<IntegerType>::value, byte&>
operator>>=(byte& b, IntegerType shift) noexcept {
  return b = b >> shift;
}

constexpr byte operator|(byte l, byte r) noexcept {
  return byte{
    static_cast<unsigned char>(
      static_cast<unsigned int>(l.operator unsigned char()) | static_cast<unsigned int>(r.operator unsigned char())
    )
  };
}

constexpr byte operator&(byte l, byte r) noexcept {
  return byte{
    static_cast<unsigned char>(
      static_cast<unsigned int>(l.operator unsigned char()) & static_cast<unsigned int>(r.operator unsigned char())
    )
  };
}

constexpr byte operator^(byte l, byte r) noexcept {
  return byte{
    static_cast<unsigned char>(
      static_cast<unsigned int>(l.operator unsigned char()) ^ static_cast<unsigned int>(r.operator unsigned char())
    )
  };
}
constexpr byte operator~(byte b) noexcept {
  return byte{
    static_cast<unsigned char>(
      ~static_cast<unsigned int>(b.operator unsigned char())
    )
  };
}

constexpr byte& operator|=(byte& l, byte r) noexcept {
  return l = l | r;
}

constexpr byte& operator&=(byte& l, byte r) noexcept {
  return l = l & r;
}

constexpr byte& operator^=(byte& l, byte r) noexcept {
  return l = l ^ r;
}

unsigned char to_underlying(byte b) {
  return b.operator unsigned char();
}

#else
using byte = std::byte;

template<typename IntegerType>
constexpr std::enable_if_t<std::is_integral<IntegerType>::value, IntegerType>
to_integer(byte b) noexcept {
  return IntegerType(b);
}

#endif

} // namespace preview

#endif // PREVIEW_CORE_BYTE_H_
