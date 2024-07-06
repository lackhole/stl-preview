//
// Created by yonggyulee on 2024/01/03.
//

#ifndef PREVIEW_RANGES_DISTANCE_H_
#define PREVIEW_RANGES_DISTANCE_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/range_difference_t.h"
#include "preview/__ranges/size.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/negation.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

struct distance_niebloid {
  template<typename I, typename S, std::enable_if_t<conjunction<
      sentinel_for<S, I>,
      negation< sized_sentinel_for<S, I> >
  >::value, int> = 0>
  constexpr iter_difference_t<I> operator()(I first, S last) const {
    using namespace preview::rel_ops;
    iter_difference_t<I> result = 0;
    while (first != last) {
      ++first;
      ++result;
    }
    return result;
  }

  template<typename I, typename S, std::enable_if_t<
      sized_sentinel_for<S, std::decay_t<I>>
      ::value, int> = 0>
  constexpr iter_difference_t<std::decay_t<I>> operator()(I&& first, S last) const {
    return last - static_cast<const std::decay_t<I>&>(first);
  }

  template<typename R, std::enable_if_t<
      range<R>
      ::value, int> = 0>
  constexpr range_difference_t<R> operator()(R&& r) const {
    return call(std::forward<R>(r), sized_range<R>{});
  }

 private:
  template<typename R>
  constexpr range_difference_t<R> call(R&& r, std::true_type) const {
    return static_cast<range_difference_t<R>>(ranges::size(r));
  }
  template<typename R>
  constexpr range_difference_t<R> call(R&& r, std::false_type) const {
    return (*this)(ranges::begin(r), ranges::end(r));
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::distance_niebloid distance{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_DISTANCE_H_
