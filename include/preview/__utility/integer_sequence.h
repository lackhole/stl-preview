//
// Created by YongGyu Lee on 2024. 7. 6.
//

#ifndef PREVIEW_UTILITY_INTEGER_SEQUENCE_H_
#define PREVIEW_UTILITY_INTEGER_SEQUENCE_H_

#include <cstddef>
#include <utility>

#include "preview/__core/inline_variable.h"

namespace preview {
namespace detail {

template<typename T, typename Seq, T N> struct integer_sequence_op_add;
template<typename T, T... I, T N>
struct integer_sequence_op_add<T, std::integer_sequence<T, I...>, N> {
  using type = std::integer_sequence<T, (I + N)...>;
};

template<typename T, T Begin, T Bound>
struct integer_sequence_ranged {
  using base_seq = std::make_integer_sequence<T, (Bound - Begin)>;
  using type = typename integer_sequence_op_add<T, base_seq, Begin>::type;
};

template<typename Seq, std::size_t N, typename Out = std::integer_sequence<typename Seq::value_type>>
struct integer_sequence_slice_left_impl;

template<typename T, T ...I, T ...J>
struct integer_sequence_slice_left_impl<std::integer_sequence<T, I...>, 0, std::integer_sequence<T, J...>> {
  using type = std::integer_sequence<T, J...>;
};
template<typename T, T I0, T ...Is, std::size_t N, T ...J>
struct integer_sequence_slice_left_impl<std::integer_sequence<T, I0, Is...>, N, std::integer_sequence<T, J...>> {
  using type = typename integer_sequence_slice_left_impl<std::integer_sequence<T, Is...>, N - 1, std::integer_sequence<T, J..., I0>>::type;
};

// Specialize for small values to get faster compilation
template<typename T, T I0, T ...Is, T ...J>
struct integer_sequence_slice_left_impl<std::integer_sequence<T, I0, Is...>, 1, std::integer_sequence<T, J...>> {
  using type = std::integer_sequence<T, J..., I0>;
};
template<typename T, T I0, T I1, T ...Is, T ...J>
struct integer_sequence_slice_left_impl<std::integer_sequence<T, I0, I1, Is...>, 2, std::integer_sequence<T, J...>> {
  using type = std::integer_sequence<T, J..., I0, I1>;
};
template<typename T, T I0, T I1, T I2, T ...Is, T ...J>
struct integer_sequence_slice_left_impl<std::integer_sequence<T, I0, I1, I2, Is...>, 3, std::integer_sequence<T, J...>> {
  using type = std::integer_sequence<T, J..., I0, I1, I2>;
};
template<typename T, T I0, T I1, T I2, T I3, T ...Is, T ...J>
struct integer_sequence_slice_left_impl<std::integer_sequence<T, I0, I1, I2, I3, Is...>, 4, std::integer_sequence<T, J...>> {
  using type = std::integer_sequence<T, J..., I0, I1, I2, I3>;
};

template<typename Seq, typename Out = std::integer_sequence<typename Seq::value_type>>
struct integer_sequence_reverse_impl;

template<typename T,  T ...J>
struct integer_sequence_reverse_impl<std::integer_sequence<T>, std::integer_sequence<T, J...>> {
  using type = std::integer_sequence<T, J...>;
};

template<typename T, T I0, T ...I, T ...J>
struct integer_sequence_reverse_impl<std::integer_sequence<T, I0, I...>, std::integer_sequence<T, J...>>
    : integer_sequence_reverse_impl<std::integer_sequence<T, I...>, std::integer_sequence<T, I0, J...>> {};
template<typename T, T I0, T I1, T ...I, T ...J>
struct integer_sequence_reverse_impl<std::integer_sequence<T, I0, I1, I...>, std::integer_sequence<T, J...>>
    : integer_sequence_reverse_impl<std::integer_sequence<T, I...>, std::integer_sequence<T, I1, I0, J...>> {};
template<typename T, T I0, T I1, T I2, T ...I, T ...J>
struct integer_sequence_reverse_impl<std::integer_sequence<T, I0, I1, I2, I...>, std::integer_sequence<T, J...>>
    : integer_sequence_reverse_impl<std::integer_sequence<T, I...>, std::integer_sequence<T, I2, I1, I0, J...>> {};
template<typename T, T I0, T I1, T I2, T I3, T ...I, T ...J>
struct integer_sequence_reverse_impl<std::integer_sequence<T, I0, I1, I2, I3, I...>, std::integer_sequence<T, J...>>
    : integer_sequence_reverse_impl<std::integer_sequence<T, I...>, std::integer_sequence<T, I3, I2, I1, I0, J...>> {};

template<std::size_t I, typename IntegerSequence>
struct integer_sequence_get_impl;

template<std::size_t I, typename T, T Int, T... Ints>
struct integer_sequence_get_impl<I, std::integer_sequence<T, Int, Ints...>>
    : integer_sequence_get_impl<I - 1, std::integer_sequence<T, Ints...>> {};

template<typename T>
struct integer_sequence_get_impl<0, std::integer_sequence<T>> {};
template<typename T, T I0, T... Ints>
struct integer_sequence_get_impl<0, std::integer_sequence<T, I0, Ints...>> : std::integral_constant<T, I0> {};
template<typename T, T I0, T I1, T... Is>
struct integer_sequence_get_impl<1, std::integer_sequence<T, I0, I1, Is...>> : std::integral_constant<T, I1> {};
template<typename T, T I0, T I1, T I2, T... Is>
struct integer_sequence_get_impl<2, std::integer_sequence<T, I0, I1, I2, Is...>> : std::integral_constant<T, I2> {};
template<typename T, T I0, T I1, T I2, T I3, T... Is>
struct integer_sequence_get_impl<3, std::integer_sequence<T, I0, I1, I2, I3, Is...>> : std::integral_constant<T, I3> {};
template<typename T, T I0, T I1, T I2, T I3, T I4, T... Is>
struct integer_sequence_get_impl<4, std::integer_sequence<T, I0, I1, I2, I3, I4, Is...>> : std::integral_constant<T, I4> {};
template<typename T, T I0, T I1, T I2, T I3, T I4, T I5, T... Is>
struct integer_sequence_get_impl<5, std::integer_sequence<T, I0, I1, I2, I3, I4, I5, Is...>> : std::integral_constant<T, I5> {};
template<typename T, T I0, T I1, T I2, T I3, T I4, T I5, T I6, T... Is>
struct integer_sequence_get_impl<6, std::integer_sequence<T, I0, I1, I2, I3, I4, I5, I6, Is...>> : std::integral_constant<T, I6> {};
template<typename T, T I0, T I1, T I2, T I3, T I4, T I5, T I6, T I7, T... Is>
struct integer_sequence_get_impl<7, std::integer_sequence<T, I0, I1, I2, I3, I4, I5, I6, I7, Is...>> : std::integral_constant<T, I7> {};

template<std::size_t I, typename IntegerSequence>
PREVIEW_INLINE_VARIABLE constexpr auto integer_sequence_get_v = integer_sequence_get_impl<I, IntegerSequence>::value;

} // namespace detail

template<typename T, T Begin, T Bound>
using make_integer_sequence = typename detail::integer_sequence_ranged<T, Begin, Bound>::type;

template<std::size_t Begin, std::size_t End>
using make_index_sequence = make_integer_sequence<std::size_t, Begin, End>;


// integer_sequence_count
template<typename T, typename Seq, T X>
struct integer_sequence_count_impl;

template<typename T, T X>
struct integer_sequence_count_impl<T, std::integer_sequence<T>, X>
    : std::integral_constant<std::size_t, 0> {};

template<typename T, T I0, T ...Is, T X>
struct integer_sequence_count_impl<T, std::integer_sequence<T, I0, Is...>, X>
    : std::integral_constant<std::size_t, (I0 == X) + integer_sequence_count_impl<T, std::integer_sequence<T, Is...>, X>::value> {};

template<typename Seq, typename Seq::value_type X>
using integer_sequence_count = integer_sequence_count_impl<typename Seq::value_type, Seq, X>;

// alias of integer_sequence<T, Ti...> where i is [0, N) (N < original-length)
template<typename Seq, std::size_t N>
using integer_sequence_slice_left = typename detail::integer_sequence_slice_left_impl<Seq, N>::type;

template<typename Seq>
using integer_sequence_reverse = typename detail::integer_sequence_reverse_impl<Seq>::type;

} // namespace preview

#endif // PREVIEW_UTILITY_INTEGER_SEQUENCE_H_
