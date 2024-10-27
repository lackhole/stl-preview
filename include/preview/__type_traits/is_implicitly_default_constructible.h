//
// Created by yonggyulee on 2024. 10. 27.
//

#ifndef PREVIEW_TYPE_TRAITS_IS_IMPLICITLY_DEFAULT_CONSTRUCTIBLE_H_
#define PREVIEW_TYPE_TRAITS_IS_IMPLICITLY_DEFAULT_CONSTRUCTIBLE_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__type_traits/void_t.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundefined-internal"

#elif defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 5046)
#endif

namespace preview {
namespace detail {

template<typename T>
struct is_implicitly_default_constructible_tester {
  void operator()(T) const noexcept;
};

template<typename T, typename = void, bool = /* false */ std::is_default_constructible<T>::value>
struct is_implicitly_default_constructible_impl : std::false_type {};

template<typename T>
struct is_implicitly_default_constructible_impl<
    T,
    void_t<decltype(is_implicitly_default_constructible_tester<const T&>{}({}))>,
    true> : std::true_type {};

} // namespace detail

template<typename T>
struct is_implicitly_default_constructible : detail::is_implicitly_default_constructible_impl<T> {};

template<typename T>
PREVIEW_INLINE_VARIABLE constexpr bool is_implicitly_default_constructible_v = is_implicitly_default_constructible<T>::value;

} // namespace preview

#ifdef __clang__
#pragma clang diagnostic pop
#elif defined(_MSC_VER)
#pragma warning( pop )
#endif

#endif // PREVIEW_TYPE_TRAITS_IS_IMPLICITLY_DEFAULT_CONSTRUCTIBLE_H_
