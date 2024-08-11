//
// Created by yonggyulee on 2024. 8. 11.
//

#ifndef PREVIEW_NUMBERS_H_
#define PREVIEW_NUMBERS_H_

#include "preview/__concepts/floating_point.h"
#include "preview/__core/inline_variable.h"

namespace preview {
namespace numbers {
namespace detail {

template<typename T, bool = floating_point<T>::value> struct e_t {};
template<typename T, bool = floating_point<T>::value> struct log2e_t {};
template<typename T, bool = floating_point<T>::value> struct log10e_t {};
template<typename T, bool = floating_point<T>::value> struct pi_t {};
template<typename T, bool = floating_point<T>::value> struct inv_pi_t {};
template<typename T, bool = floating_point<T>::value> struct inv_sqrtpi_t {};
template<typename T, bool = floating_point<T>::value> struct ln2_t {};
template<typename T, bool = floating_point<T>::value> struct ln10_t {};
template<typename T, bool = floating_point<T>::value> struct sqrt2_t {};
template<typename T, bool = floating_point<T>::value> struct sqrt3_t {};
template<typename T, bool = floating_point<T>::value> struct inv_sqrt3_t {};
template<typename T, bool = floating_point<T>::value> struct egamma_t {};
template<typename T, bool = floating_point<T>::value> struct phi_t {};

template<typename T> struct e_t<T, true>          { static constexpr T value = static_cast<T>(2.71828182845904523536); };
template<typename T> struct log2e_t<T, true>      { static constexpr T value = static_cast<T>(1.44269504088896340735); };
template<typename T> struct log10e_t<T, true>     { static constexpr T value = static_cast<T>(0.43429448190325182765); };
template<typename T> struct pi_t<T, true>         { static constexpr T value = static_cast<T>(3.14159265358979323846); };
template<typename T> struct inv_pi_t<T, true>     { static constexpr T value = static_cast<T>(0.31830988618379067153); };
template<typename T> struct inv_sqrtpi_t<T, true> { static constexpr T value = static_cast<T>(0.56418958354775628694); };
template<typename T> struct ln2_t<T, true>        { static constexpr T value = static_cast<T>(0.69314718055994530941); };
template<typename T> struct ln10_t<T, true>       { static constexpr T value = static_cast<T>(2.30258509299404568401); };
template<typename T> struct sqrt2_t<T, true>      { static constexpr T value = static_cast<T>(1.41421356237309504880); };
template<typename T> struct sqrt3_t<T, true>      { static constexpr T value = static_cast<T>(1.73205080756887729352); };
template<typename T> struct inv_sqrt3_t<T, true>  { static constexpr T value = static_cast<T>(0.57735026918962576450); };
template<typename T> struct egamma_t<T, true>     { static constexpr T value = static_cast<T>(0.57721566490153286060); };
template<typename T> struct phi_t<T, true>        { static constexpr T value = static_cast<T>(1.61803398874989484820); };

} // namespace detail

template<typename T> PREVIEW_INLINE_VARIABLE constexpr T e_v          = detail::e_t<T>::value;
template<typename T> PREVIEW_INLINE_VARIABLE constexpr T log2e_v      = detail::log2e_t<T>::value;
template<typename T> PREVIEW_INLINE_VARIABLE constexpr T log10e_v     = detail::log10e_t<T>::value;
template<typename T> PREVIEW_INLINE_VARIABLE constexpr T pi_v         = detail::pi_t<T>::value;
template<typename T> PREVIEW_INLINE_VARIABLE constexpr T inv_pi_v     = detail::inv_pi_t<T>::value;
template<typename T> PREVIEW_INLINE_VARIABLE constexpr T inv_sqrtpi_v = detail::inv_sqrtpi_t<T>::value;
template<typename T> PREVIEW_INLINE_VARIABLE constexpr T ln2_v        = detail::ln2_t<T>::value;
template<typename T> PREVIEW_INLINE_VARIABLE constexpr T ln10_v       = detail::ln10_t<T>::value;
template<typename T> PREVIEW_INLINE_VARIABLE constexpr T sqrt2_v      = detail::sqrt2_t<T>::value;
template<typename T> PREVIEW_INLINE_VARIABLE constexpr T sqrt3_v      = detail::sqrt3_t<T>::value;
template<typename T> PREVIEW_INLINE_VARIABLE constexpr T inv_sqrt3_v  = detail::inv_sqrt3_t<T>::value;
template<typename T> PREVIEW_INLINE_VARIABLE constexpr T egamma_v     = detail::egamma_t<T>::value;
template<typename T> PREVIEW_INLINE_VARIABLE constexpr T phi_v        = detail::phi_t<T>::value;

PREVIEW_INLINE_VARIABLE constexpr double e          = e_v<double>;
PREVIEW_INLINE_VARIABLE constexpr double log2e      = log2e_v<double>;
PREVIEW_INLINE_VARIABLE constexpr double log10e     = log10e_v<double>;
PREVIEW_INLINE_VARIABLE constexpr double pi         = pi_v<double>;
PREVIEW_INLINE_VARIABLE constexpr double inv_pi     = inv_pi_v<double>;
PREVIEW_INLINE_VARIABLE constexpr double inv_sqrtpi = inv_sqrtpi_v<double>;
PREVIEW_INLINE_VARIABLE constexpr double ln2        = ln2_v<double>;
PREVIEW_INLINE_VARIABLE constexpr double ln10       = ln10_v<double>;
PREVIEW_INLINE_VARIABLE constexpr double sqrt2      = sqrt2_v<double>;
PREVIEW_INLINE_VARIABLE constexpr double sqrt3      = sqrt3_v<double>;
PREVIEW_INLINE_VARIABLE constexpr double inv_sqrt3  = inv_sqrt3_v<double>;
PREVIEW_INLINE_VARIABLE constexpr double egamma     = egamma_v<double>;
PREVIEW_INLINE_VARIABLE constexpr double phi        = phi_v<double>;

} // namespace numbers
} // namespace preview

#endif // PREVIEW_NUMBERS_H_
