//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_CBEGIN_H_
#define PREVIEW_RANGES_CBEGIN_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/basic_const_iterator.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__ranges/possibly_const_range.h"
#include "preview/__type_traits/disjunction.h"

namespace preview {
namespace ranges {
namespace detail {

struct cbegin_niebloid {
  template<typename T, std::enable_if_t<disjunction<
      std::is_lvalue_reference<T>,
      enable_borrowed_range<std::remove_cv_t<T>>
  >::value, int> = 0>
  constexpr auto operator()(T&& t) const {
    return const_iterator<decltype(ranges::begin(preview::ranges::possibly_const_range(t)))>(
                                   ranges::begin(preview::ranges::possibly_const_range(t)));
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::cbegin_niebloid cbegin{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_CBEGIN_H_
