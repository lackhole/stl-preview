//
// Created by yonggyulee on 2023/12/07.
//

#ifndef PREVIEW_MEMORY_TO_ADDRESS_H_
#define PREVIEW_MEMORY_TO_ADDRESS_H_

#include <memory>
#include <type_traits>

#include "preview/__memory/pointer_traits.h"
#include "preview/__type_traits/detail/tag.h"
#include "preview/__type_traits/has_operator_arrow.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename T, template<typename> class Traits, typename = void>
struct has_to_address : std::false_type {};

template<typename T, template<typename> class Traits>
struct has_to_address<T, Traits, void_t<decltype(Traits<T>::to_address(std::declval<const T&>()))>> : std::true_type {};

template<typename T>
using to_address_tag = conditional_tag<
#if defined(_MSC_VER) && _MSC_VER < 1930 // std::pointer_traits is not sfinae-friendly before VS 2022
    has_to_address<T, pointer_traits>
#else
    has_to_address<T, std::pointer_traits>,
    has_to_address<T, pointer_traits>
#endif
>;

template<typename T>
constexpr auto to_address_fancy(const T& p, preview::detail::tag_1 /* std::pointer_traits<T>::to_address */ ) noexcept {
  return std::pointer_traits<T>::to_address(p);
}
template<typename T>
constexpr auto to_address_fancy(const T& p, preview::detail::tag_2 /* pointer_traits<T>::to_address */ ) noexcept {
  return pointer_traits<T>::to_address(p);
}
template<typename T>
constexpr auto to_address_fancy(const T& p, preview::detail::tag_else /* no to_address */ ) noexcept;

} // namespace detail

template<class T>
constexpr T* to_address(T* p) noexcept {
  static_assert(!std::is_function<T>::value, "T must not be a pointer to function");
  return p;
}

template<class T>
constexpr auto to_address(const T& p) noexcept {
  return detail::to_address_fancy(p, detail::to_address_tag<T>{});
}

namespace detail {

template<typename T>
constexpr auto to_address_fancy(const T& p, preview::detail::tag_else /* no to_address */ ) noexcept {
  return preview::to_address(p.operator->());
}

} // namespace detail

} // namespace preview

#endif // PREVIEW_MEMORY_TO_ADDRESS_H_
