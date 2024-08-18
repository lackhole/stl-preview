//
// Created by yonggyulee on 2024/01/30.
//

#ifndef PREVIEW_RANGES_VIEWS_REPEAT_H_
#define PREVIEW_RANGES_VIEWS_REPEAT_H_

#include <type_traits>

#include "preview/__concepts/constructible_from.h"
#include "preview/__concepts/integer_like.h"
#include "preview/__concepts/move_constructible.h"
#include "preview/__concepts/same_as.h"
#include "preview/__concepts/semiregular.h"
#include "preview/__core/inline_variable.h"
#include "preview/__ranges/range_adaptor.h"
#include "preview/__ranges/views/repeat_view.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

struct repeat_niebloid {
  template<typename W, std::enable_if_t<conjunction<
      move_constructible< std::decay_t<W> >,
      std::is_object    < std::decay_t<W> >,
      same_as           < std::decay_t<W>,
                          std::remove_cv_t<std::decay_t<W>> >
  >::value, int> = 0>
  constexpr auto operator()(W&& value) const {
    return repeat_view<std::decay_t<W>>{std::forward<W>(value)};
  }

  template<typename W, typename Bound, std::enable_if_t<conjunction<
      move_constructible< std::decay_t<W>     >,
      semiregular       < std::decay_t<Bound> >,

      std::is_object    < std::decay_t<W> >,
      same_as           < std::decay_t<W>,
                          std::remove_cv_t<std::decay_t<W>> >,
      disjunction<
          ranges::detail::integer_like_with_usable_difference_type< std::decay_t<Bound> >,
          same_as< std::decay_t<Bound>, unreachable_sentinel_t >
      >
  >::value, int> = 0>
  constexpr auto operator()(W&& value, Bound&& bound) const {
    return repeat_view<std::decay_t<W>, std::decay_t<Bound>>{std::forward<W>(value), std::forward<Bound>(bound)};
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::repeat_niebloid repeat{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_REPEAT_H_
