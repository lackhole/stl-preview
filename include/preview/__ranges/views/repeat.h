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
  template<typename T, std::enable_if_t<conjunction<
      move_constructible< std::decay_t<T> >,
      std::is_object    < std::decay_t<T> >,
      same_as           < std::decay_t<T>,
                          std::remove_cv_t<std::decay_t<T>> >
  >::value, int> = 0>
  constexpr auto operator()(T&& value) const {
    return repeat_view<std::decay_t<T>>{std::forward<T>(value)};
  }

  template<typename T, typename Bound, std::enable_if_t<conjunction<
      move_constructible< std::decay_t<T>     >,
      semiregular       < std::decay_t<Bound> >,

      std::is_object    < std::decay_t<T> >,
      same_as           < std::decay_t<T>,
                          std::remove_cv_t<std::decay_t<T>> >,
      disjunction<
          ranges::detail::integer_like_with_usable_difference_type< std::decay_t<Bound> >,
          same_as< std::decay_t<Bound>, unreachable_sentinel_t >
      >
  >::value, int> = 0>
  constexpr auto operator()(T&& value, Bound&& bound) const {
    return repeat_view<std::decay_t<T>, std::decay_t<Bound>>{std::forward<T>(value), std::forward<Bound>(bound)};
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::repeat_niebloid repeat{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_REPEAT_H_
