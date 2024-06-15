//
// Created by yonggyulee on 1/24/24.
//

#ifndef PREVIEW_RANGES_VIEWS_FILTER_H_
#define PREVIEW_RANGES_VIEWS_FILTER_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__ranges/range_adaptor.h"
#include "preview/__ranges/range_adaptor_closure.h"
#include "preview/__ranges/views/filter_view.h"
#include "preview/__ranges/viewable_range.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

struct filter_niebloid {
  template<typename R, typename Pred, std::enable_if_t<viewable_range<R>::value, int> = 0>
  constexpr auto operator()(R&& r, Pred&& pred) const {
    return filter_view<all_t<R>, remove_cvref_t<Pred>>(std::forward<R>(r), std::forward<Pred>(pred));
  }

  template<typename Pred>
  constexpr auto operator()(Pred&& pred) const {
    return range_adaptor<filter_niebloid, std::decay_t<Pred>>{std::forward<Pred>(pred)};
  }

};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::filter_niebloid filter{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_FILTER_H_
