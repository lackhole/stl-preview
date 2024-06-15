//
// Created by yonggyulee on 2/3/24.
//

#ifndef PREVIEW_RANGES_VIEWS_BASIC_ISTREAM_H_
#define PREVIEW_RANGES_VIEWS_BASIC_ISTREAM_H_

#include <type_traits>

#include "preview/__concepts/derived_from.h"
#include "preview/__core/inline_variable.h"
#include "preview/__ranges/views/basic_istream_view.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

template<typename T, typename = void>
struct has_typename_char_type : std::false_type {};
template<typename T>
struct has_typename_char_type<T, void_t<typename T::char_type>> : std::true_type {};

template<typename T, typename = void>
struct has_typename_traits_type : std::false_type {};
template<typename T>
struct has_typename_traits_type<T, void_t<typename T::traits_type>> : std::true_type {};

template<typename U, bool = conjunction<has_typename_char_type<U>, has_typename_traits_type<U>>::value /* false */>
struct derived_from_basic_istream : std::false_type {};
template<typename U>
struct derived_from_basic_istream<U, true>
    : derived_from<U, std::basic_istream<typename U::char_type, typename U::traits_type>> {};

template<typename T>
struct basic_istream_niebloid {
  template<typename E, std::enable_if_t<
      derived_from_basic_istream<std::remove_reference_t<E>
  >::value, int> = 0>
  constexpr auto operator()(E&& e) const {
    using U = std::remove_reference_t<decltype(e)>;
    return basic_istream_view<T, typename U::char_type, typename U::traits_type>(std::forward<E>(e));
  }
};

} // namespace detail

template<typename T>
PREVIEW_INLINE_VARIABLE constexpr detail::basic_istream_niebloid<T> istream{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_BASIC_ISTREAM_H_
