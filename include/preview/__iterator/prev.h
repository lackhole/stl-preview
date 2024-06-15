//
// Created by cosge on 2023-12-25.
//

#ifndef PREVIEW_ITERATOR_PREV_H_
#define PREVIEW_ITERATOR_PREV_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/bidirectional_iterator.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/advance.h"

namespace preview {
namespace ranges {
namespace detail {

struct prev_niebloid {
  template<typename I>
  constexpr std::enable_if_t<bidirectional_iterator<I>::value, I>
  operator()(I i) const {
    --i;
    return i;
  }

  template<typename I>
  constexpr std::enable_if_t<bidirectional_iterator<I>::value, I>
  operator()(I i, iter_difference_t<I> n) const {
    ranges::advance(i, -n);
    return i;
  }


  template<typename I>
  constexpr std::enable_if_t<bidirectional_iterator<I>::value, I>
  operator()(I i, iter_difference_t<I> n, I bound) const {
    ranges::advance(i, -n, bound);
    return i;
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::prev_niebloid prev{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ITERATOR_PREV_H_
