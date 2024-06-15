//
// Created by YongGyu Lee on 11/3/23.
//

#ifndef PREVIEW_VARIANT_VARIANT_ALTERNATIVE_H_
#define PREVIEW_VARIANT_VARIANT_ALTERNATIVE_H_

#include <cstddef>
#include <type_traits>

#include "preview/__utility/type_sequence.h"

namespace preview {

template<typename... Types>
class variant;

template<std::size_t, typename T>
struct variant_alternative;

template <std::size_t I, class T>
using variant_alternative_t = typename variant_alternative<I, T>::type;

template<std::size_t I, typename T>
struct variant_alternative<I, const T> {
  using type = std::add_const_t<variant_alternative_t<I, T>>;
};

template<std::size_t I, typename... Types>
struct variant_alternative<I, variant<Types...>>
    : type_sequence_element_type<I, type_sequence<Types...>> {};

} // namespace preview

#endif // PREVIEW_VARIANT_VARIANT_ALTERNATIVE_H_
