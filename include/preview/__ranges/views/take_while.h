//
// Created by YongGyu Lee on 3/27/24.
//

#ifndef PREVIEW_RANGES_VIEWS_TAKE_WHILE_H_
#define PREVIEW_RANGES_VIEWS_TAKE_WHILE_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/indirect_unary_predicate.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/range_adaptor.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/viewable_range.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/take_while_view.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

struct take_while_niebloid {
  template<typename R, typename Pred, std::enable_if_t<conjunction<
      has_typename_type < detail::all_t_impl<R> >,
      view              < all_t<R>              >,
      input_range       < all_t<R>              >,
      std::is_object    < std::decay_t<Pred>    >,
      indirect_unary_predicate<const std::decay_t<Pred>, iterator_t<all_t<R>>>
  >::value, int> = 0>
  constexpr auto operator()(R&& r, Pred&& pred) const {
    return take_while_view<all_t<R>, std::decay_t<Pred>>(std::forward<R>(r), std::forward<Pred>(pred));
  }

  template<typename Pred, std::enable_if_t<
      std::is_object<std::decay_t<Pred>>
  ::value, int> = 0>
  constexpr auto operator()(Pred&& pred) const {
    return range_adaptor<take_while_niebloid, std::decay_t<Pred>>(std::forward<Pred>(pred));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::take_while_niebloid take_while{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_TAKE_WHILE_H_
