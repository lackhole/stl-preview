//
// Created by yonggyulee on 1/12/24.
//

#ifndef PREVIEW_UTILITY_TYPE_SEQUENCE_H_
#define PREVIEW_UTILITY_TYPE_SEQUENCE_H_

#include <cstddef>

namespace preview {

template<typename...>
struct type_sequence;

namespace detail {

template<typename S1, typename S2>
struct type_sequence_cat_impl;
template<typename... Ts, typename... Us>
struct type_sequence_cat_impl<type_sequence<Ts...>, type_sequence<Us...>> {
  using type = type_sequence<Ts..., Us...>;
};
template<typename S1, typename S2>
using type_sequence_cat_t = typename type_sequence_cat_impl<S1, S2>::type;

template<typename T, std::size_t>
struct make_type_sequence_impl;

template<typename T, std::size_t N> struct type_sequence_impl;
template<typename T> struct make_type_sequence_impl<T, 0> { using type = type_sequence<>; };
template<typename T> struct make_type_sequence_impl<T, 1> { using type = type_sequence<T>; };
template<typename T> struct make_type_sequence_impl<T, 2> { using type = type_sequence<T, T>; };
template<typename T, std::size_t N>
struct make_type_sequence_impl {
  using type = type_sequence_cat_t<typename make_type_sequence_impl<T, (N / 2)>::type,
                                   typename make_type_sequence_impl<T, N - (N / 2)>::type>;
};

template<std::size_t N, typename T, typename TS>
struct type_sequence_type_count_impl;

template<std::size_t N, typename T>
struct type_sequence_type_count_impl<N, T, type_sequence<>>
    : std::integral_constant<std::size_t, N> {};

template<std::size_t N, typename T, typename U1, typename... Us>
struct type_sequence_type_count_impl<N, T, type_sequence<U1, Us...>>
    : type_sequence_type_count_impl<N, T, type_sequence<Us...>> {};

template<std::size_t N, typename T , typename... Us>
struct type_sequence_type_count_impl<N, T, type_sequence<T, Us...>>
    : type_sequence_type_count_impl<N + 1, T, type_sequence<Us...>> {};

template<std::size_t I, typename T, typename TS>
struct type_sequence_type_index_impl;

template<std::size_t I, typename T>
struct type_sequence_type_index_impl<I, T, type_sequence<>>
    : std::integral_constant<std::size_t, I + 1> {};

template<std::size_t I, typename T, typename U1, typename... Us>
struct type_sequence_type_index_impl<I, T, type_sequence<U1, Us...>>
    : type_sequence_type_index_impl<I + 1, T, type_sequence<Us...>> {};

template<std::size_t I, typename T, typename... Us>
struct type_sequence_type_index_impl<I, T, type_sequence<T, Us...>>
    : std::integral_constant<std::size_t, I> {};

} // namespace detail

// create a `type_sequence` that holds `T` of `N` times
template<typename T, std::size_t N>
using make_type_sequence = typename detail::make_type_sequence_impl<T, N>::type;

// count the number of occurrences of `T` in `type_sequence`
template<typename T, typename TS>
struct type_sequence_type_count : detail::type_sequence_type_count_impl<0, T, TS> {};

// get the index of `T` in `type_sequence`.
// value is `sizeof...(U)` if `T` is not found in `type_sequence`
template<typename T, typename TS>
struct type_sequence_type_index : detail::type_sequence_type_index_impl<0, T, TS> {};


// get type of \f$I\f$-th element of `type_sequence`
template<std::size_t I, typename TS>
struct type_sequence_element_type;

template<std::size_t I>
struct type_sequence_element_type<I, type_sequence<>> {};

template<typename U, typename... Us>
struct type_sequence_element_type<0, type_sequence<U, Us...>> {
  using type = U;
};

template<std::size_t I, typename U, typename... Us>
struct type_sequence_element_type<I, type_sequence<U, Us...>>
    : type_sequence_element_type<I - 1, type_sequence<Us...>> {};

template<std::size_t I, typename TS>
using type_sequence_element_type_t = typename type_sequence_element_type<I, TS>::type;

template<typename... Types>
struct type_sequence {
  template<typename T>
  static constexpr std::size_t count = type_sequence_type_count<T, type_sequence>::value;

  template<typename T>
  static constexpr bool unique = (type_sequence_type_count<T, type_sequence>::value == 1);

  template<typename T>
  static constexpr std::size_t index = type_sequence_type_index<T, type_sequence>::value;

  template<std::size_t I>
  using element_type = type_sequence_element_type_t<I, type_sequence>;

  using front = element_type<0>;
  using back = element_type<sizeof...(Types) - 1>;
};

} // namespace preview

#endif // PREVIEW_UTILITY_TYPE_SEQUENCE_H_
