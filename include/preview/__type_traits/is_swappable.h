//
// Created by yonggyulee on 2023/09/29.
//

#ifndef PREVIEW_TYPE_TRAITS_IS_SWAPPABLE_H_
#define PREVIEW_TYPE_TRAITS_IS_SWAPPABLE_H_

#include <utility>
#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__core/std_version.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_referenceable.h"
#include "preview/__type_traits/void_t.h"

namespace preview {

template<typename T> struct is_swappable;
template<typename T> struct is_nothrow_swappable;

namespace internal {

namespace swappable_test_std {

# if __cplusplus < 201703L

template<typename T, typename U>
struct is_swappable_with_impl : conjunction<
    std::is_convertible<decltype(std::declval<U>()), decltype(std::declval<T>())>,
    std::is_convertible<decltype(std::declval<T>()), decltype(std::declval<U>())>,
    std::is_move_constructible<std::remove_reference_t<T>>, std::is_move_assignable<std::remove_reference_t<T>>,
    std::is_move_constructible<std::remove_reference_t<U>>, std::is_move_assignable<std::remove_reference_t<U>>
    > {
  static constexpr bool nothrow =
      std::is_nothrow_move_constructible<std::remove_reference_t<T>>::value && std::is_nothrow_move_assignable<std::remove_reference_t<T>>::value &&
      std::is_nothrow_move_constructible<std::remove_reference_t<U>>::value && std::is_nothrow_move_assignable<std::remove_reference_t<U>>::value;
};

template<typename T, std::size_t N>
struct is_swappable_with_impl<T(&)[N], T(&)[N]> : is_swappable<T> {
  static constexpr bool nothrow = is_nothrow_swappable<T>::value;
};

# else
using std::swap;
template<typename T, typename U, typename = void>
struct is_swappable_with_impl : std::false_type {};

template<typename T, typename U>
struct is_swappable_with_impl<T, U, void_t<
    decltype((swap(std::declval<T>(), std::declval<U>()),
              swap(std::declval<U>(), std::declval<T>()))
            )>
    > : std::true_type {
  static constexpr bool nothrow
      = noexcept(swap(std::declval<T>(), std::declval<U>())) &&
        noexcept(swap(std::declval<U>(), std::declval<T>()));

};

# endif

} // namespace swappable_test_std

namespace swappable_test_adl {

template<typename T, typename U, typename = void>
struct is_swappable_with_impl : std::false_type {};

template<typename T, typename U>
struct is_swappable_with_impl<T, U, void_t<
    decltype((swap(std::declval<T>(), std::declval<U>()),
              swap(std::declval<U>(), std::declval<T>())))>> : std::true_type {
  static constexpr bool nothrow
      = noexcept(swap(std::declval<T>(), std::declval<U>())) &&
        noexcept(swap(std::declval<U>(), std::declval<T>()));
};

} // namespace swappable_test_adl

#if PREVIEW_CXX_VERSION < 17

template<typename T, typename U>
struct is_swappable_with_impl :
    disjunction<swappable_test_adl::is_swappable_with_impl<T, U>,
                swappable_test_std::is_swappable_with_impl<T, U>> {};

template<typename T, typename U, bool v = is_swappable_with_impl<T, U>::value>
struct is_nothrow_swappable_with_impl : bool_constant<is_swappable_with_impl<T, U>::nothrow> {};

template<typename T, typename U>
struct is_nothrow_swappable_with_impl<T, U, false> : std::false_type {};

#else

template<typename T, typename U>
struct is_swappable_with_impl : std::is_swappable_with<T, U> {};

template<typename T, typename U>
struct is_nothrow_swappable_with_impl : std::is_nothrow_swappable_with<T, U> {};

#endif

} // namespace internal

template<typename T, typename U>
struct is_swappable_with : internal::is_swappable_with_impl<T, U> {};

template<typename T, typename U>
PREVIEW_INLINE_VARIABLE constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;

template<typename T>
struct is_swappable :
    std::conditional_t<
        !is_referencable<T>::value,
        std::false_type,
        is_swappable_with<std::add_lvalue_reference_t<T>, std::add_lvalue_reference_t<T>>
    > {};

template<typename T>
PREVIEW_INLINE_VARIABLE constexpr bool is_swappable_v = is_swappable<T>::value;

template<typename T, typename U>
struct is_nothrow_swappable_with : internal::is_nothrow_swappable_with_impl<T, U> {};

template<typename T, typename U>
PREVIEW_INLINE_VARIABLE constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T, U>::value;

template<typename T>
struct is_nothrow_swappable : is_nothrow_swappable_with<T&, T&> {};

template<typename T>
PREVIEW_INLINE_VARIABLE constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;

} // namespace preview

# if PREVIEW_CXX_VERSION < 17

#include "preview/__type_traits/detail/is_swappable_cxx14.h"

# endif

#endif // PREVIEW_TYPE_TRAITS_IS_SWAPPABLE_H_
