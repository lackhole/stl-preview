//
// Created by YongGyu Lee on 4/15/24.
//

#ifndef PREVIEW_ITERATOR_REVERSE_ITERATOR_H_
#define PREVIEW_ITERATOR_REVERSE_ITERATOR_H_


#include <iterator>
#include <type_traits>

#include "preview/__concepts/derived_from.h"
#include "preview/__iterator/indirectly_swappable.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/iter_move.h"
#include "preview/__iterator/iter_reference_t.h"
#include "preview/__iterator/iter_rvalue_reference_t.h"
#include "preview/__iterator/iter_swap.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/random_access_iterator.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__type_traits/negation.h"

namespace preview {

template<typename I1, typename I2>
struct disable_sized_sentinel_for<std::reverse_iterator<I1>, std::reverse_iterator<I2>>
    : negation<sized_sentinel_for<I1, I2>> {};

// iterator_traits
#if __cplusplus < 202002L

template<typename Iter>
struct iterator_traits<std::reverse_iterator<Iter>> {
  using iterator_concept = std::conditional_t<
      random_access_iterator<Iter>::value,
      random_access_iterator_tag,
      bidirectional_iterator_tag
  >;
  using iterator_category = std::conditional_t<
      derived_from<typename iterator_traits<Iter>::iterator_category, random_access_iterator_tag>::value,
      random_access_iterator_tag,
      typename iterator_traits<Iter>::iterator_category
  >;
  using value_type = iter_value_t<Iter>;
  using difference_type = iter_difference_t<Iter>;
  using pointer = typename iterator_traits<Iter>::pointer;
  using reference = iter_reference_t<Iter>;
};

namespace detail {

template<typename Iter>
struct is_specialized_iterator_traits<iterator_traits<std::reverse_iterator<Iter>>> : std::false_type {};

} // namespace detail

#endif // __cplusplus < 202002L

} // namespace preview

// iter_move, iter_swap
#if __cplusplus < 202002L

namespace std {

template<typename Iter>
constexpr preview::iter_rvalue_reference_t<Iter> iter_move(const std::reverse_iterator<Iter>& i)
    noexcept(
        std::is_nothrow_copy_constructible<Iter>::value &&
        noexcept(preview::ranges::iter_move(--std::declval<Iter&>()))
    )
{
  auto tmp = i.base();
  return preview::ranges::iter_move(--tmp);
}

template<typename Iter, typename Iter2, std::enable_if_t<preview::indirectly_swappable<Iter2, Iter>::value, int> = 0>
constexpr void iter_swap(const reverse_iterator<Iter>& x, const reverse_iterator<Iter2>& y)
    noexcept(
        std::is_nothrow_copy_constructible<Iter>::value &&
        std::is_nothrow_copy_constructible<Iter2>::value &&
        noexcept(preview::ranges::iter_swap(--std::declval<Iter&>(), --std::declval<Iter2&>()))
    )
{
  auto tmp_x = x.base();
  auto tmp_y = y.base();
  preview::ranges::iter_swap(--tmp_x, --tmp_y);
}

} // namespace std

#endif // __cplusplus < 202002L

#endif // PREVIEW_ITERATOR_REVERSE_ITERATOR_H_
