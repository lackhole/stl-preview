//
// Created by cosge on 2024-01-13.
//

#ifndef PREVIEW_ITERATOR_NEXT_H_
#define PREVIEW_ITERATOR_NEXT_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/advance.h"
#include "preview/__iterator/input_or_output_iterator.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/sentinel_for.h"

namespace preview {
namespace ranges {
namespace detail {

struct next_niebloid {
  template<typename I>
  constexpr std::enable_if_t<input_or_output_iterator<I>::value, I>
  operator()(I i) const {
    ++i;
    return i;
  }

  template<typename I, std::enable_if_t<input_or_output_iterator<I>::value, int> = 0>
  constexpr I operator()(I i, iter_difference_t<I> n) const {
    ranges::advance(i, n);
    return i;
  }

  template<typename I, typename S, std::enable_if_t<conjunction<
      input_or_output_iterator<I>,
      sentinel_for<S, I>
  >::value, int> = 0>
  constexpr I operator()(I i, S bound) const {
    ranges::advance(i, bound);
    return i;
  }

  template<typename I, typename S, std::enable_if_t<conjunction<
      input_or_output_iterator<I>,
      sentinel_for<S, I>
  >::value, int> = 0>
  constexpr I operator()(I i, iter_difference_t<I> n, S bound) const {
    ranges::advance(i, n, bound);
    return i;
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::next_niebloid next{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ITERATOR_NEXT_H_
