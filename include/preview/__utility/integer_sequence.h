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

} // namespace detail

template<typename T, T Begin, T Bound>
using make_integer_sequence = typename detail::integer_sequence_ranged<T, Begin, Bound>::type;

template<std::size_t Begin, std::size_t End>
using make_index_sequence = make_integer_sequence<std::size_t, Begin, End>;

} // namespace preview

#endif // PREVIEW_UTILITY_INTEGER_SEQUENCE_H_
