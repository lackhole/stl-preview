//
// Created by yonggyulee on 2023/12/27.
//

#ifndef PREVIEW_ITERATOR_ITER_SWAP_H_
#define PREVIEW_ITERATOR_ITER_SWAP_H_

#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__concepts/swappable_with.h"
#include "preview/__iterator/detail/iter_exchange_move.h"
#include "preview/__iterator/indirectly_movable_storable.h"
#include "preview/__iterator/indirectly_readable.h"
#include "preview/__iterator/iter_reference_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/is_class_or_enum.h"

namespace preview {
namespace ranges {
namespace detail_iter_swap {

template<typename T, typename U>
void iter_swap(T, U) = delete;

template<typename T, typename U>
constexpr auto test_iter_swap(int) -> decltype(iter_swap(std::declval<T>(), std::declval<U>()), std::true_type{});

template<typename T, typename U>
constexpr auto test_iter_swap(...) -> std::false_type;

template<typename T, typename U, bool = disjunction<
    is_class_or_enum<remove_cvref_t<T>>,
    is_class_or_enum<remove_cvref_t<U>>
  >::value /* false */>
struct unqualified_iter_swappable : std::false_type {};

template<typename T, typename U>
struct unqualified_iter_swappable<T, U, true> : decltype(test_iter_swap<T, U>(0)) {};

template<typename I1, typename I2, bool = conjunction<indirectly_readable<I1>, indirectly_readable<I2>>::value /* false */>
struct iter_deref_swappable : std::false_type{};
template<typename I1, typename I2>
struct iter_deref_swappable<I1, I2, true> : swappable_with<iter_reference_t<I1>, iter_reference_t<I2>> {};

struct iter_swap_niebloid {
  template<typename I1, typename I2, std::enable_if_t<unqualified_iter_swappable<I1, I2>::value, int> = 0>
  constexpr void operator()(I1&& i1, I2&& i2) const
      noexcept(noexcept(iter_swap(std::forward<I1>(i1), std::forward<I2>(i2))))
  {
    (void)iter_swap(std::forward<I1>(i1), std::forward<I2>(i2));
  }

  template<typename I1, typename I2, std::enable_if_t<conjunction<
      negation< unqualified_iter_swappable<I1, I2> >,
      iter_deref_swappable<I1, I2>
  >::value, int> = 0>
  constexpr void operator()(I1&& i1, I2&& i2) const
      noexcept(noexcept(ranges::swap(*i1, *i2)))
  {
    ranges::swap(*std::forward<I1>(i1), *std::forward<I2>(i2));
  }

  template<typename I1, typename I2, std::enable_if_t<conjunction<
      negation< unqualified_iter_swappable<I1, I2> >,
      negation< iter_deref_swappable<I1, I2> >,
      indirectly_movable_storable<I1, I2>,
      indirectly_movable_storable<I2, I1>
  >::value, int> = 0>
  constexpr void operator()(I1&& i1, I2&& i2) const
    noexcept(noexcept( *i1 = preview::detail::iter_exchange_move(std::forward<I2>(i2), std::forward<I1>(i1)) ))
  {
    (void)(*i1 = preview::detail::iter_exchange_move(std::forward<I2>(i2), std::forward<I1>(i1)));
  }
};

} // namespace detail_iter_swap

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail_iter_swap::iter_swap_niebloid iter_swap{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ITERATOR_ITER_SWAP_H_
