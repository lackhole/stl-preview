//
// Created by YongGyu Lee on 12/1/23.
//

#ifndef PREVIEW_FUNCTIONAL_HASH_ARRAY_H_
#define PREVIEW_FUNCTIONAL_HASH_ARRAY_H_

#include <climits>
#include <cstddef>
#include <type_traits>

namespace preview {

// Implementation taken from https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

template<typename T, std::size_t Bytes = sizeof(T) * CHAR_BIT>
struct basic_FNV_prime;
template<typename T>
struct basic_FNV_prime<T, 32> : std::integral_constant<T, 0x01000193> {};
template<typename T>
struct basic_FNV_prime<T, 64> : std::integral_constant<T, 0x00000100000001B3> {};

using FNV_prime = basic_FNV_prime<std::size_t>;

template<typename T, std::size_t Bytes = sizeof(T) * CHAR_BIT>
struct basic_FNV_offset_basis;
template<typename T>
struct basic_FNV_offset_basis<T, 32> : std::integral_constant<T, 0x811c9dc5> {};
template<typename T>
struct basic_FNV_offset_basis<T, 64> : std::integral_constant<T, 0xcbf29ce484222325> {};

using FNV_offset_basis = basic_FNV_offset_basis<std::size_t>;


// template<typename T> struct basic_FNV_prime<T, 128> : std::integral_constant<T, 0x0000000001000000000000000000013B> {};
// template<typename T> struct basic_FNV_offset_basis<T, 128> : std::integral_constant<T, 0x6c62272e07bb014262b821756295c58d> {};

// template<typename T> struct basic_FNV_prime<T, 256>
//     : std::integral_constant<T, 0x0000000000000000000001000000000000000000000000000000000000000163> {};
// template<typename T> struct basic_FNV_offset_basis<T, 256>
//     : std::integral_constant<T, 0xdd268dbcaac550362d98c384c4e576ccc8b1536847b6bbb31023b4c8caee0535> {};

template<typename T>
std::size_t FNV_1(std::size_t value, const T& byte) {
  static_assert(std::is_integral<T>::value, "T must integral type");
  value = value * FNV_prime::value;
  value = value ^ static_cast<std::size_t>(byte);
  return value;
}

inline std::size_t FNV_1(std::size_t value, const unsigned char* const bytes, std::size_t size) {
  for (std::size_t i = 0; i < size; ++i) {
    value = FNV_1(value, bytes[i]);
  }
  return value;
}

template<typename T>
std::size_t FNV_1a(std::size_t value, const T& byte) {
  static_assert(std::is_integral<T>::value, "T must integral type");
  value = value ^ static_cast<std::size_t>(byte);
  value = value * FNV_prime::value;
  return value;
}

inline std::size_t FNV_1a(std::size_t value, const unsigned char* const bytes, std::size_t size) {
  for (std::size_t i = 0; i < size; ++i) {
    value = FNV_1a(value, bytes[i]);
  }
  return value;
}

template<typename T>
std::size_t hash_array(const T* bytes, std::size_t size) {
  return FNV_1a(FNV_offset_basis::value, reinterpret_cast<const unsigned char* const>(bytes), sizeof(T) * size);
}


template<typename T, std::size_t N>
std::size_t hash_array(const T(&bytes)[N]) {
  return hash_array(bytes, N);
}

} // namespace preview

#endif // PREVIEW_FUNCTIONAL_HASH_ARRAY_H_
