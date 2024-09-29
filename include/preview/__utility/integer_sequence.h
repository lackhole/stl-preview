//
// Created by YongGyu Lee on 2024. 7. 6.
//

#ifndef PREVIEW_UTILITY_INTEGER_SEQUENCE_H_
#define PREVIEW_UTILITY_INTEGER_SEQUENCE_H_

#include <cstddef>
#include <utility>

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

} // namespace detail

template<typename T, T Begin, T Bound>
using make_integer_sequence = typename detail::integer_sequence_ranged<T, Begin, Bound>::type;

template<std::size_t Begin, std::size_t End>
using make_index_sequence = make_integer_sequence<std::size_t, Begin, End>;


// integer_sequence_count
template<typename Seq, typename Seq::value_type X>
struct integer_sequence_count;

template<typename T, T X>
struct integer_sequence_count<std::integer_sequence<T>, X>
    : std::integral_constant<std::size_t, 0> {};

template<typename T, T I0, T ...Is, T X>
struct integer_sequence_count<std::integer_sequence<T, I0, Is...>, X>
    : std::integral_constant<std::size_t, (I0 == X) + integer_sequence_count<std::integer_sequence<T, Is...>, X>::value> {};

// alias of integer_sequence<T, Ti...> where i is [0, N) (N < original-length)
template<typename Seq, std::size_t N>
using integer_sequence_slice_left = typename detail::integer_sequence_slice_left_impl<Seq, N>::type;

} // namespace preview

#endif // PREVIEW_UTILITY_INTEGER_SEQUENCE_H_
