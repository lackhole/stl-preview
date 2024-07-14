//
// Created by cosge on 2023-12-25.
//

#ifndef PREVIEW_ITERATOR_ADVANCE_H_
#define PREVIEW_ITERATOR_ADVANCE_H_

#include <type_traits>

#include "preview/__concepts/assignable_from.h"
#include "preview/__core/inline_variable.h"
#include "preview/__iterator/bidirectional_iterator.h"
#include "preview/__iterator/input_or_output_iterator.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/random_access_iterator.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__type_traits/detail/tag.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/negation.h"

namespace preview {
namespace ranges {
namespace detail {

struct advance_niebloid {
 private:
  template<typename I, typename S, bool = has_typename_type<iter_difference<I>>::value /* false */>
  struct convertible_to_iter_difference : std::false_type {};
  template<typename I, typename S>
  struct convertible_to_iter_difference<I, S, true> : convertible_to<S, iter_difference_t<I>> {};

 public:
  template<typename I>
  constexpr std::enable_if_t<input_or_output_iterator<I>::value>
  operator()(I& i, iter_difference_t<I> n) const {
    using tag_t = preview::detail::conditional_tag<random_access_iterator<I>, bidirectional_iterator<I>>;
    advance_n(i, n, tag_t{});
  }

  template<typename I, typename S, std::enable_if_t<conjunction<
      input_or_output_iterator<I>,
      negation<convertible_to_iter_difference<I, S>>,
      sentinel_for<S, I>
  >::value, int> = 0>
  constexpr void operator()(I& i, S bound) const {
    using tag_t = preview::detail::conditional_tag<assignable_from<I&, S>, sized_sentinel_for<S, I>>;
    advance_bound(i, bound, tag_t{});
  }

  template<typename I, typename S, std::enable_if_t<conjunction<
      input_or_output_iterator<I>,
      sentinel_for<S, I>
  >::value, int> = 0>
  constexpr iter_difference_t<I>
  operator()(I& i, iter_difference_t<I> n, S bound) const {
    using tag_t = preview::detail::conditional_tag<sized_sentinel_for<S, I>, bidirectional_iterator<I>>;
    return advance_mixed(i, n, bound, tag_t{});
  }

 private:
  using tag_1 = preview::detail::tag_1;
  using tag_2 = preview::detail::tag_2;
  using tag_else = preview::detail::tag_else;

  template<typename I>
  constexpr void advance_n(I& i, iter_difference_t<I> n, tag_1 /* random_access_iterator */) const {
    i += n;
  }

  template<typename I>
  constexpr void advance_n(I& i, iter_difference_t<I> n, tag_2 /* bidirectional_iterator */) {
    while (n > 0) {
      --n;
      ++i;
    }

    while (n < 0) {
      ++n;
      --i;
    }
  }

  template<typename I>
  constexpr void advance_n(I& i, iter_difference_t<I> n, tag_else) const {
    while (n > 0) {
      --n;
      ++i;
    }
  }

  template<typename I, typename S>
  constexpr void advance_bound(I& i, S bound, tag_1 /* assignable_from */) const {
    i = std::move(bound);
  }

  template<typename I, typename S>
  constexpr void advance_bound(I& i, S bound, tag_2 /* sized_sentinel_for */) const {
    (*this)(i, bound - i);
  }

  template<typename I, typename S>
  constexpr void advance_bound(I& i, S bound, tag_else) const {
    while (i != bound)
      ++i;
  }

  template<typename I, typename S>
  constexpr iter_difference_t<I>
  advance_mixed(I& i, iter_difference_t<I> n, S bound, tag_1 /* sized_sentinel_for */) const {
    const iter_difference_t<I> d = bound - i;
    if ((n < 0 && n <= d) || (n > 0 && n >= d)) {
      (*this)(i, d);
      return n - d;
    }

    (*this)(i, n);
    return 0;
  }

  template<typename I, typename S>
  constexpr iter_difference_t<I>
  advance_mixed(I& i, iter_difference_t<I> n, S bound, tag_2 /* bidirectional_iterator */) const {
    while (n > 0 && i != bound) {
      --n;
      ++i;
    }

    while (n < 0 && i != bound) {
      ++n;
      --i;
    }

    return n;
  }

  template<typename I, typename S>
  constexpr iter_difference_t<I>
  advance_mixed(I& i, iter_difference_t<I> n, S bound, tag_else) const {
    while (n > 0 && i != bound) {
      --n;
      ++i;
    }

    return n;
  }
};
} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::advance_niebloid advance{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ITERATOR_ADVANCE_H_
