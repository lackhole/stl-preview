//
// Created by yonggyulee on 2/4/24.
//

#ifndef PREVIEW_VARIANTS_HOLDS_ALTERNATIVE_H_
#define PREVIEW_VARIANTS_HOLDS_ALTERNATIVE_H_

#include <type_traits>

#include "preview/__utility/type_sequence.h"
#include "preview/__variant/variant.h"

namespace preview {
namespace detail {

template<typename T>
struct variant_holds_alternative_visitor {
  template<typename U, std::size_t I>
  constexpr bool operator()(const U&, in_place_index_t<I>) const {
    return I != variant_npos && std::is_same<T, U>::value;
  }
};

} // namespace detail

template<typename T, typename... Types, std::enable_if_t<
    (type_sequence<Types...>::template count<T> == 1)
, int> = 0>
constexpr bool holds_alternative(const variant<Types...>& v) noexcept {
  return detail::variant_raw_visit(v.index(), v._base().union_, detail::variant_holds_alternative_visitor<T>{});
}

} // namespace preview

#endif // PREVIEW_VARIANTS_HOLDS_ALTERNATIVE_H_
