//
// Created by yonggyulee on 09/05/2025
//

#ifndef PREVIEW_ALGORITHM_RANGES_MOVE_H_
#define PREVIEW_ALGORITHM_RANGES_MOVE_H_

#include <type_traits>
#include <utility>

#include "preview/config.h"
#include "preview/__algorithm/ranges/in_out_result.h"
#include "preview/__core/inline_variable.h"
#include "preview/__iterator/indirectly_movable.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/iter_move.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__iterator/weakly_incrementable.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {

template<typename I, typename O>
using move_result = in_out_result<I, O>;

namespace detail {

struct move_niebloid {
  template<typename I, typename S, typename O, std::enable_if_t<conjunction_v<
      input_iterator<I>,
      sentinel_for<S, I>,
      weakly_incrementable<O>,
      // requires
      indirectly_movable<I, O>
  >, int> = 0>
  constexpr move_result<I, O> operator()(I first, S last, O result) const {
#if !PREVIEW_CONFORM_CXX20_STANDARD
    using namespace preview::rel_ops;
#endif
    for (; first != last; ++first, (void)++result) {
      *result = ranges::iter_move(first);
    }
    return {std::move(first), std::move(result)};
  }

  template<typename R, typename O, std::enable_if_t<conjunction_v<
      input_range<R>,
      weakly_incrementable<O>,
      // requires
      indirectly_movable<iterator_t<R>, O>
  >, int> = 0>
  constexpr move_result<borrowed_iterator_t<R>, O> operator()(R&& r, O result) const {
    return (*this)(ranges::begin(r), ranges::end(r), std::move(result));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::move_niebloid move{};

} // namespace ranges
} // namespace preview 

#endif // PREVIEW_ALGORITHM_RANGES_MOVE_H_
