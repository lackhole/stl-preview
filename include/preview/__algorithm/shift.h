//
// Created by YongGyu Lee on 1/25/24.
//

#ifndef PREVIEW_ALGORITHM_SHIFT_H_
#define PREVIEW_ALGORITHM_SHIFT_H_

#include <algorithm>
#include <type_traits>
#include <utility>

#include "preview/__concepts/value_swappable.h"
#include "preview/__iterator/advance.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/iterator_traits/legacy_bidirectional_iterator.h"
#include "preview/__iterator/iterator_traits/legacy_forward_iterator.h"
#include "preview/__type_traits/disjunction.h"

namespace preview {
namespace detail {

template<typename It>
constexpr void move_nocheck(It first, It last, It dest) {
  while (first != last) {
    *dest = std::move(*first);
    ++first;
  }
}

} // namespace detail

template<typename ForwardIt>
constexpr ForwardIt shift_left(
    ForwardIt first, ForwardIt last, typename iterator_traits<ForwardIt>::difference_type n)
{
  static_assert(LegacyForwardIterator<ForwardIt>::value, "Constraints not satisfied");
  static_assert(disjunction<
      LegacyBidirectionalIterator<ForwardIt>,
      ValueSwappable<ForwardIt, ForwardIt>>::value, "Constraints not satisfied");

  if (n == 0)
    return last;

  ForwardIt shift_first = first;
  const auto d = ranges::advance(shift_first, n, last);
  if (d != 0)
    return first;

  for (; shift_first != last; ++shift_first, (void) ++first) {
    *first = std::move(*shift_first);
  }
  return first;
}

namespace detail {

template<typename BidiIt>
constexpr BidiIt shift_right_impl_bidi(
    BidiIt first, BidiIt last,
    typename iterator_traits<BidiIt>::difference_type n,
    std::true_type /* RandomAccessIt */)
{
  if (n >= last - first)
    return last;
  auto shift_last = last - n;
  while (first != shift_last) {
    *--last = std::move(*--shift_last);
  }
  return first;
}

template<typename BidiIt>
constexpr BidiIt shift_right_impl_bidi(
    BidiIt first, BidiIt last,
    typename iterator_traits<BidiIt>::difference_type n,
    std::false_type /* RandomAccessIt */)
{
  auto shift_last = last;
  for (; n > 0; --n) {
    if (shift_last == first)
      return last;
    --shift_last;
  }
  while (first != shift_last) {
    *--last = std::move(*--shift_last);
  }
  return first;
}

template<typename BidiIt>
constexpr BidiIt shift_right_impl(
    BidiIt first, BidiIt last,
    typename iterator_traits<BidiIt>::difference_type n,
    std::true_type /* BidirectionalIt */)
{
  return shift_right_impl_bidi(first, last, n, LegacyRandomAccessIterator<BidiIt>{});
}

template<typename ForwardIt>
constexpr ForwardIt shift_right_impl(
    ForwardIt first, ForwardIt last,
    typename iterator_traits<ForwardIt>::difference_type n,
    std::false_type /* BidirectionalIt */)
{
  auto result = first;
  for (; n > 0; --n) {
    if (result == last)
      return last;
    ++result;
  }

  auto trail = first;
  auto lead  = result;

  for (; trail != result; ++trail, (void) ++lead) {
    if (lead == last) {
      preview::detail::move_nocheck(first, trail, result);
      return first;
    }
  }

  for (;;) {
    for (auto mid = first; mid != result; ++mid, (void) ++trail, ++lead) {
      if (lead == last) {
        trail = preview::detail::move_nocheck(mid, result, trail);
        preview::detail::move_nocheck(first, mid, trail);

        return first;
      }
      using namespace std;
      swap(*mid, *trail);
    }
  }
  // Unreachable
}

} // namespace detail

template<typename ForwardIt>
constexpr ForwardIt shift_right(
    ForwardIt first, ForwardIt last, typename iterator_traits<ForwardIt>::difference_type n)
{
  static_assert(LegacyForwardIterator<ForwardIt>::value, "Constraints not satisfied");
  static_assert(disjunction<
      LegacyBidirectionalIterator<ForwardIt>,
      ValueSwappable<ForwardIt, ForwardIt>>::value, "Constraints not satisfied");

  return detail::shift_right_impl(first, last, n, LegacyBidirectionalIterator<ForwardIt>{});
}

} // namespace preview

#endif // PREVIEW_ALGORITHM_SHIFT_H_
