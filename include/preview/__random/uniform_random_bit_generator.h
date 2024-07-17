//
// Created by yonggyulee on 2024-07-16.
//

#ifndef PREVIEW_RANDOM_UNIFORM_RANDOM_BIT_GENERATOR_H_
#define PREVIEW_RANDOM_UNIFORM_RANDOM_BIT_GENERATOR_H_

#include <type_traits>

#include "preview/__concepts/integral.h"
#include "preview/__concepts/invocable.h"
#include "preview/__concepts/same_as.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename T, typename R, typename = void>
struct has_constexpr_static_fn_min : std::false_type {};
template<typename T, typename R>
struct has_constexpr_static_fn_min<T, R, void_t<std::integral_constant<decltype(T::min()), T::min()>>>
    : same_as<R, decltype(T::min())> {};

template<typename T, typename R, typename = void>
struct has_constexpr_static_fn_max : std::false_type {};
template<typename T, typename R>
struct has_constexpr_static_fn_max<T, R, void_t<std::integral_constant<decltype(T::max()), T::max()>>>
    : same_as<R, decltype(T::max())> {};

template<typename T>
struct min_less_max : bool_constant<(T::min() < T::max())> {};

template<typename G, bool = /* false */ conjunction<invocable<G&>>::value>
struct uniform_random_bit_generator_impl : std::false_type {};

template<typename G>
struct uniform_random_bit_generator_impl<G, true> :
    conjunction<
        unsigned_integral<invoke_result_t<G&>>,
        detail::has_constexpr_static_fn_min<G, invoke_result_t<G&>>,
        detail::has_constexpr_static_fn_max<G, invoke_result_t<G&>>,
        detail::min_less_max<G>
    > {};

} // namespace detail

template<typename G>
struct uniform_random_bit_generator : detail::uniform_random_bit_generator_impl<G> {};

} // namespace preview

#endif // PREVIEW_RANDOM_UNIFORM_RANDOM_BIT_GENERATOR_H_
