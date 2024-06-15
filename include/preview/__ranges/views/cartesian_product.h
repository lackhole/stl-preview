//
// Created by yonggyulee on 2024/01/27.
//

#ifndef PREVIEW_RANGES_VIEWS_CARTESIAN_PRODUCT_H_
#define PREVIEW_RANGES_VIEWS_CARTESIAN_PRODUCT_H_

#include <tuple>
#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__ranges/viewable_range.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/cartesian_product_view.h"
#include "preview/__ranges/views/single.h"

#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

struct cartesian_product_niebloid {
  constexpr auto operator()() const {
    return views::single(std::tuple<>());
  }

  template<typename... Rs, std::enable_if_t<conjunction<
      viewable_range<Rs>...
  >::value, int> = 0>
  constexpr auto operator()(Rs&&... rs) const {
    return ranges::cartesian_product_view<all_t<Rs&&>...>(std::forward<Rs>(rs)...);
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::cartesian_product_niebloid cartesian_product{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_CARTESIAN_PRODUCT_H_
