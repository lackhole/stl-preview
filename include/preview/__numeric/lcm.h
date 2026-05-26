//
// Created by Chi-Iroh on 12/12/2025.
//

#ifndef PREVIEW_NUMERIC_LCM_H_
#define PREVIEW_NUMERIC_LCM_H_

#include <type_traits>
#include "preview/__numeric/lcm.h"

namespace preview {
namespace detail{

template<typename T, typename U>
constexpr std::enable_if_t<!std::is_same<T, bool>::value && !std::is_same<U, bool>::value && std::is_integral<T>::value && std::is_integral<U>::value, std::common_type_t<T, U>> _lcm(T a, U b) {
    return a * (b / preview::gcd<T, U>(a, b));
}
} // namespace detail

template<typename T, typename U>
constexpr std::enable_if_t<!std::is_same<T, bool>::value && !std::is_same<U, bool>::value && std::is_integral<T>::value && std::is_integral<U>::value, std::common_type_t<T, U>> lcm(T a, U b) {
    return detail::_lcm<std::make_signed_t<T>, std::make_signed_t<U>>(a < 0 ? -a : a, b < 0 ? -b : b);
}
} // namespace preview

#endif