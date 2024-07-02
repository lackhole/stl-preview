//
// Created by yonggyulee on 2/3/24.
//

#ifndef PREVIEW_RANGES_VIEWS_SPLIT_H_
#define PREVIEW_RANGES_VIEWS_SPLIT_H_

#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__ranges/range_adaptor.h"
#include "preview/__ranges/viewable_range.h"
#include "preview/__ranges/views/split_view.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

struct split_niebloid {
  template<typename R, typename Pattern, std::enable_if_t<viewable_range<R>::value, int> = 0>
  constexpr auto operator()(R&& r, Pattern&& pattern) const {
    return preview::ranges::make_split_view(std::forward<R>(r), std::forward<Pattern>(pattern));
  }

  template<typename Pattern>
  constexpr auto operator()(Pattern&& pattern) const {
    return range_adaptor<split_niebloid, std::decay_t<Pattern>>{std::forward<Pattern>(pattern)};
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::split_niebloid split{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_SPLIT_H_
