//
// Created by YongGyu Lee on 4/15/24.
//

#ifndef PREVIEW_ITERATOR_MOVE_ITERATOR_H_
#define PREVIEW_ITERATOR_MOVE_ITERATOR_H_

#include <iterator>
#include <type_traits>

#include "preview/__core/constexpr.h"
#include "preview/__concepts/derived_from.h"
#include "preview/__iterator/bidirectional_iterator.h"
#include "preview/__iterator/detail/have_cxx20_iterator.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/indirectly_swappable.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/iter_move.h"
#include "preview/__iterator/iter_rvalue_reference_t.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__iterator/move_sentinel.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__iterator/random_access_iterator.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/negation.h"

namespace preview {

template<typename Iterator1, typename Iterator2>
struct disable_sized_sentinel_for<std::move_iterator<Iterator1>, std::move_iterator<Iterator2>>
    : negation<sized_sentinel_for<Iterator1, Iterator2>> {};

#if __cplusplus < 202302L

namespace detail {

template<typename Iter, bool = has_typename_iterator_category<Iter>::value /* false */>
struct move_iterator_category {};

template<typename Iter>
struct move_iterator_category<Iter, true> {
  using iterator_category = std::conditional_t<
      derived_from<typename iterator_traits<Iter>::iterator_category, random_access_iterator_tag>::value, random_access_iterator_tag,
      typename iterator_traits<Iter>::iterator_category
  >;
};

} // namespace detail

template<typename Iter>
struct iterator_traits<std::move_iterator<Iter>>
    : detail::move_iterator_category<Iter>
{
  using iterator_concept = conditional_t<
      random_access_iterator<Iter>, random_access_iterator_tag,
      bidirectional_iterator<Iter>, bidirectional_iterator_tag,
      forward_iterator<Iter>, forward_iterator_tag,
      input_iterator_tag
  >;
  using value_type = iter_value_t<Iter>;
  using difference_type = iter_difference_t<Iter>;
  using pointer = Iter;
  using reference = iter_rvalue_reference_t<Iter>;
};

namespace detail {

template<typename Iter>
struct is_specialized_iterator_traits<std::move_iterator<Iter>> : std::false_type {};

} // namespace detail

#endif // __cplusplus < 202302L

} // namespace preview

#if !PREVIEW_STD_HAVE_CXX20_ITERATOR

namespace std {

template<typename Iter>
constexpr preview::iter_rvalue_reference_t<Iter> iter_move(const move_iterator<Iter>& i)
    noexcept(noexcept(preview::ranges::iter_move(i.base())))
{
  return preview::ranges::iter_move(i.base());
}

template<typename Iter, typename Iter2, std::enable_if_t<preview::indirectly_swappable<Iter, Iter2>::value, int> = 0>
constexpr void iter_swap(const move_iterator<Iter>& x, const move_iterator<Iter2>& y)
    noexcept(noexcept(preview::ranges::iter_swap(x.base(), y.base())))
{
  preview::ranges::iter_swap(x.base(), y.base());
}

} // namespace std

#endif // !PREVIEW_STD_HAVE_CXX20_ITERATOR

#endif // PREVIEW_ITERATOR_MOVE_ITERATOR_H_
