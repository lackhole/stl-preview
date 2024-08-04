//
// Created by yonggyulee on 2024. 8. 4.
//

#ifndef PREVIEW_BIT_BIT_CAST_H_
#define PREVIEW_BIT_BIT_CAST_H_

#include <type_traits>

#include "preview/config.h"

#if PREVIEW_CONFORM_CXX20_STANDARD
#include <bit>
#else
#include <cstring>
#endif

#include "preview/__memory/addressof.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename To>
struct bit_cast_helper {
  bit_cast_helper() {}

  union {
    char object_bytes[sizeof(To)];
    To object;
  };
};

template<typename To, typename From, std::enable_if_t<std::is_trivially_constructible<To>::value == false, int> = 0>
To bit_cast_impl(const From& from) noexcept {
  bit_cast_helper<To> storage;
  std::memcpy(storage.object_bytes, preview::addressof(from), sizeof(From));
  return storage.object;
}

template<typename To, typename From, std::enable_if_t<std::is_trivially_constructible<To>::value, int> = 0>
To bit_cast_impl(const From& from) noexcept {
  To to;
  std::memcpy(preview::addressof(to), preview::addressof(from), sizeof(From));
  return to;
}

} // namespace detail

template<typename To, typename From, std::enable_if_t<conjunction<
    bool_constant<sizeof(To) == sizeof(From)>,
    std::is_trivially_copyable<To>,
    std::is_trivially_copyable<From>
>::value, int> = 0>
constexpr To bit_cast(const From& from) noexcept {
#if PREVIEW_HAVE_BUILTIN_BIT_CAST
  return __builtin_bit_cast(To, from);
#elif PREVIEW_CONFORM_CXX20_STANDARD
  return std::bit_cast<To>(from);
#else
  return preview::detail::bit_cast_impl<To>(from);
#endif
}

} // namespace preview

#endif // PREVIEW_BIT_BIT_CAST_H_
