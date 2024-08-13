//
// Created by yonggyulee on 2024. 8. 14.
//

#ifndef PREVIEW_NUMERIC_RANGES_IOTA_H_
#define PREVIEW_NUMERIC_RANGES_IOTA_H_

#include <type_traits>
#include <utility>

#include "preview/__algorithm/ranges/out_value_result.h"
#include "preview/__core/inline_variable.h"
#include "preview/__iterator/indirectly_writable.h"
#include "preview/__iterator/input_or_output_iterator.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__iterator/weakly_incrementable.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/output_range.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__utility/as_const.h"

namespace preview {
namespace ranges {

template<typename O, typename T>
using iota_result = out_value_result<O, T>;

namespace detail {

struct iota_niebloid {
  template<typename O, typename S, typename T, std::enable_if_t<conjunction<
      input_or_output_iterator<O>,
      sentinel_for<S, O>,
      weakly_incrementable<T>,
      indirectly_writable<O, const T&>
  >::value, int> = 0>
  constexpr iota_result<O, T> operator()(O first, S last, T value) const {
    while (first != last) {
      *first = preview::as_const(value);
      ++first;
      ++value;
    }
    return {std::move(first), std::move(value)};
  }

  template<typename T, typename R, std::enable_if_t<conjunction<
      weakly_incrementable<T>,
      output_range<R, const T&>,
      indirectly_writable<borrowed_iterator_t<R>, T>
  >::value, int> = 0>
  constexpr iota_result<borrowed_iterator_t<R>, T> operator()(R&& r, T value) const {
    return (*this)(ranges::begin(r), ranges::end(r), std::move(value));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::iota_niebloid iota{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_NUMERIC_RANGES_IOTA_H_
