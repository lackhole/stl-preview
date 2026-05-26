//
// Created by Chi-Iroh on 12/12/2025.
//

#ifndef PREVIEW_NUMERIC_GCD_H_
#define PREVIEW_NUMERIC_GCD_H_

#include <type_traits>

namespace preview {
namespace detail{

template<typename T, typename U>
constexpr std::enable_if_t<std::is_integral<T>::value && std::is_integral<U>::value, std::common_type_t<T, U>> _gcd(T a, U b) {
    while (b) {
        const U t{ b };
        b = a % b;
        a = t;
    }
    return a;
}
} // namespace detail

template<typename T, typename U>
constexpr std::enable_if_t<!std::is_same<T, bool>::value && !std::is_same<U, bool>::value && std::is_integral<T>::value && std::is_integral<U>::value, std::common_type_t<T, U>> gcd(T a, U b) {
    return detail::_gcd<std::make_signed_t<T>, std::make_signed_t<U>>(a < 0 ? -a : a, b < 0 ? -b : b);
}
} // namespace preview

#endif