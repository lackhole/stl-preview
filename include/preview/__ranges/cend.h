//
// Created by yonggyulee on 2024/01/16.
//

#ifndef PREVIEW_RANGES_CEND_H_
#define PREVIEW_RANGES_CEND_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/basic_const_iterator.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/possibly_const_range.h"
#include "preview/__type_traits/disjunction.h"

namespace preview {
namespace ranges {
namespace detail {

struct cend_niebloid {
  template<typename T, std::enable_if_t<disjunction<
      std::is_lvalue_reference<T>,
      enable_borrowed_range_t<std::remove_cv_t<T>>
  >::value, int> = 0>
  constexpr auto operator()(T&& t) const {
    return preview::const_sentinel<decltype(ranges::end(possibly_const_range(t)))>(ranges::end(possibly_const_range(t)));
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::cend_niebloid cend{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_CEND_H_
