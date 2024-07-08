//
// Created by yonggyulee on 1/12/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_COPY_H_
#define PREVIEW_ALGORITHM_RANGES_COPY_H_

#include <type_traits>
#include <utility>

#include "preview/__algorithm/ranges/in_out_result.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/identity.h"
#include "preview/__iterator/indirectly_copyable.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__iterator/weakly_incrementable.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace ranges {

template<typename I, typename O> using copy_result = in_out_result<I, O>;
template<typename I, typename O> using copy_if_result = in_out_result<I, O>;

namespace detail {

struct copy_niebloid {
  template<typename I, typename S, typename O, std::enable_if_t<conjunction<
    input_iterator<I>,
    sentinel_for<S, I>,
    weakly_incrementable<O>,
    indirectly_copyable<I, O>
  >::value, int> = 0>
  constexpr copy_result<I, O> operator()(I first, S last, O result) const {
    for (; first != last; ++first, (void)++result)
      *result = *first;
    return {std::move(first), std::move(result)};
  }

  template<typename R, typename O, std::enable_if_t<conjunction<
    input_range<R>,
    weakly_incrementable<O>,
    indirectly_copyable<iterator_t<R>, O>,
    has_typename_type<borrowed_iterator<R>>
  >::value, int> = 0>
  constexpr copy_result<borrowed_iterator_t<R>, O> operator()(R&& r, O result) const {
    return (*this)(ranges::begin(r), ranges::end(r), std::move(result));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::copy_niebloid copy{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_COPY_H_
