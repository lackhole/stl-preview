//
// Created by lux on 2024. 7. 14..
//

#ifndef PREVIEW_TYPE_TRAITS_MAKE_SIGNED_LIKE_H_
#define PREVIEW_TYPE_TRAITS_MAKE_SIGNED_LIKE_H_

#include <cstdint>
#include <type_traits>

#include "preview/__concepts/integer_like.h"
#include "preview/__concepts/integer_class.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conditional.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundefined-internal"

#elif defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 5046)
#endif

namespace preview {
namespace detail {

template<typename T, bool = integer_like<T>::value /* false */>
struct make_signed_like {};

template<typename T>
struct make_signed_like<T, true> {
 private:
  using fallback =
      conditional_t<
          bool_constant<sizeof(T) == sizeof(std::int8_t)>,  std::integral_constant<std::int8_t, 0>,
          bool_constant<sizeof(T) == sizeof(std::int16_t)>, std::integral_constant<std::int16_t, 1>,
          bool_constant<sizeof(T) == sizeof(std::int32_t)>, std::integral_constant<std::int32_t, 2>,
          bool_constant<sizeof(T) == sizeof(std::int64_t)>, std::integral_constant<std::int64_t, 3>,
          std::integral_constant<std::intmax_t, 4>
      >;

  template<typename U>
  static constexpr auto call(std::true_type, U) -> std::make_signed_t<T>;

  // deprecated attribute doesn't mean it's deprecated. It is to generate warning in cross-compiler manner

  [[deprecated("Cannot deduce corresponding unsigned type of T. Fallback to std::int8_t")]]
  static constexpr auto call(std::false_type, std::integral_constant<std::int8_t , 0>) -> std::int8_t;

  [[deprecated("Cannot deduce corresponding unsigned type of T. Fallback to std::int16_t")]]
  static constexpr auto call(std::false_type, std::integral_constant<std::int16_t, 1>) -> std::int16_t;

  [[deprecated("Cannot deduce corresponding unsigned type of T. Fallback to std::int32_t")]]
  static constexpr auto call(std::false_type, std::integral_constant<std::int32_t, 2>) -> std::int32_t;

  [[deprecated("Cannot deduce corresponding unsigned type of T. Fallback to std::int64_t")]]
  static constexpr auto call(std::false_type, std::integral_constant<std::int64_t, 3>) -> std::int64_t;

  [[deprecated("Cannot deduce corresponding unsigned type of T. Fallback to std::intmax_t")]]
  static constexpr auto call(std::false_type, std::integral_constant<std::intmax_t, 4>) -> std::intmax_t;

 public:
  using type = decltype(call(integral<T>{}, fallback{}));
};

} // namespace detail

template<typename T>
using make_signed_like_t = typename detail::make_signed_like<T>::type;

} // namespace preview

#ifdef __clang__
#pragma clang diagnostic pop
#elif defined(_MSC_VER)
#pragma warning( pop )
#endif

#endif // PREVIEW_TYPE_TRAITS_MAKE_SIGNED_LIKE_H_
