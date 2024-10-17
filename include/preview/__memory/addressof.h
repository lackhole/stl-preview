//
// Created by yonggyulee on 1/28/24.
//

#ifndef PREVIEW_MEMORY_ADDRESSOF_H_
#define PREVIEW_MEMORY_ADDRESSOF_H_

#include <memory>
#include <type_traits>

#include "preview/core.h"
#include "preview/config.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

PREVIEW_INLINE_VARIABLE constexpr int builtin_addressof_tester{};

template<typename = void>
struct have_builtin_addressof_test : std::false_type {};

template<>
struct have_builtin_addressof_test<
    void_t<
        decltype(std::integral_constant<
            bool,
            (&builtin_addressof_tester == __builtin_addressof(builtin_addressof_tester))
        >{})
    >
> : std::true_type {};

using have_builtin_addressof = have_builtin_addressof_test<>;

template<typename T>
constexpr T* addressof_object(T& t, std::true_type) noexcept {
  return __builtin_addressof(t);
}

template<typename T>
T* addressof_object(T& t, std::false_type) noexcept {
  return reinterpret_cast<T*>(
      &const_cast<char&>(
          reinterpret_cast<const volatile char&>(t)
      )
  );
}

template<typename T>
constexpr T* addressof_impl(T& t, std::true_type /* is_object */) noexcept {
  return preview::detail::addressof_object(t, have_builtin_addressof{});
}

template<typename T>
constexpr T* addressof_impl(T& t, std::false_type /* is_object */) noexcept {
  return &t;
}

} // namespace detail

# if PREVIEW_CXX_VERSION >= 17
template<typename T>
constexpr T* addressof(T& t) noexcept {
  return std::addressof(t);
}
# else
#  if PREVIEW_HAVE_BUILTIN_CONSTEXPR_ADDRESSOF
template<typename T>
constexpr T* addressof(T& t) noexcept {
  return __builtin_addressof(t);
}
#  else
template<typename T>
constexpr T* addressof(T& t) noexcept {
  return preview::detail::addressof_impl(t, std::is_object<T>{});
}
#  endif
# endif

template<typename T>
constexpr const T* addressof(T&&) = delete;

} // namespace preview

#endif // PREVIEW_MEMORY_ADDRESSOF_H_
