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

struct constexpr_min_tester {
  template<typename T>
  static constexpr auto test(int)
      -> decltype(std::integral_constant<decltype(T::min()), T::min()>{}, std::true_type{});
  template<typename R>
  static constexpr auto test(...) -> std::false_type;
};
struct constexpr_max_tester {
  template<typename T>
  static constexpr auto test(int)
      -> decltype(std::integral_constant<decltype(T::max()), T::max()>{}, std::true_type{});
  template<typename R>
  static constexpr auto test(...) -> std::false_type;
};

template<typename T>
struct has_constexpr_static_fn_min_impl
  : decltype(constexpr_min_tester::test<std::remove_reference_t<T>>(0)) {};
template<typename T, typename R, bool = /* false */ has_constexpr_static_fn_min_impl<T>::value>
struct has_constexpr_static_fn_min : std::false_type {};
template<typename T, typename R>
struct has_constexpr_static_fn_min<T, R, true> : same_as<decltype(T::min()), R> {};

template<typename T>
struct has_constexpr_static_fn_max_impl
  : decltype(constexpr_max_tester::test<std::remove_reference_t<T>>(0)) {};
template<typename T, typename R, bool = /* false */ has_constexpr_static_fn_max_impl<T>::value>
struct has_constexpr_static_fn_max : std::false_type {};
template<typename T, typename R>
struct has_constexpr_static_fn_max<T, R, true> : same_as<decltype(T::max()), R> {};

template<typename T>
struct min_less_max : bool_constant<(T::min() < T::max())> {};

template<typename G, bool = /* false */ conjunction<
    unsigned_integral<invoke_result_t<G&>>,
    detail::has_constexpr_static_fn_min<G, invoke_result_t<G&>>,
    detail::has_constexpr_static_fn_max<G, invoke_result_t<G&>>
>::value>
struct uniform_random_bit_generator_impl_2 : std::false_type {};

template<typename G>
struct uniform_random_bit_generator_impl_2<G, true> : detail::min_less_max<G> {};

template<typename G, bool = /* false */ conjunction<invocable<G&>>::value>
struct uniform_random_bit_generator_impl : std::false_type {};

template<typename G>
struct uniform_random_bit_generator_impl<G, true> : uniform_random_bit_generator_impl_2<G> {};

} // namespace detail

template<typename G>
struct uniform_random_bit_generator : detail::uniform_random_bit_generator_impl<G> {};

} // namespace preview

#endif // PREVIEW_RANDOM_UNIFORM_RANDOM_BIT_GENERATOR_H_
