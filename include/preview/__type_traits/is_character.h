#ifndef PREVIEW_TYPE_TRAITS_IS_CHARACTER_H_
#define PREVIEW_TYPE_TRAITS_IS_CHARACTER_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__core/std_version.h"

namespace preview {

template<typename T>
struct is_character : std::false_type {};

template<> struct is_character<char> : std::true_type {};
template<> struct is_character<signed char> : std::true_type {};
template<> struct is_character<unsigned char> : std::true_type {};
template<> struct is_character<wchar_t> : std::true_type {};
template<> struct is_character<char16_t> : std::true_type {};
template<> struct is_character<char32_t> : std::true_type {};

#if PREVIEW_CXX_VERSION >= 20
template<> struct is_character<char8_t> : std::true_type {};
#endif // PREVIEW_CXX_VERSION >= 20

template<typename... B>
PREVIEW_INLINE_VARIABLE constexpr bool is_character_v = is_character<B...>::value;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_IS_CHARACTER_H_
