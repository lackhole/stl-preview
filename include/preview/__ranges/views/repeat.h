//
// Created by yonggyulee on 2024/01/30.
//

#ifndef PREVIEW_RANGES_VIEWS_REPEAT_H_
#define PREVIEW_RANGES_VIEWS_REPEAT_H_

#include <type_traits>

#include "preview/__concepts/constructible_from.h"
#include "preview/__concepts/move_constructible.h"
#include "preview/__concepts/same_as.h"
#include "preview/__concepts/semiregular.h"
#include "preview/__core/inline_variable.h"
#include "preview/__ranges/views/repeat_view.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/is_integer_like.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

struct repeat_niebloid {
  template<typename W, std::enable_if_t<conjunction<
      move_constructible< remove_cvref_t<W> >,
      std::is_object< remove_cvref_t<W> >
  >::value, int> = 0>
  constexpr auto operator()(W&& value) const {
    using RV = repeat_view<remove_cvref_t<W>>;
    return RV(std::forward<W>(value));
  }

  template<typename W, typename Bound, std::enable_if_t<conjunction<
      move_constructible< remove_cvref_t<W> >,
      std::is_object< remove_cvref_t<W> >,
      disjunction<
          is_integer_like< remove_cvref_t<Bound> >,
          same_as< remove_cvref_t<Bound>, unreachable_sentinel_t >
      >
  >::value, int> = 0>
  constexpr auto operator()(W&& value, Bound&& bound) const {
    using RV = repeat_view<remove_cvref_t<W>, remove_cvref_t<Bound>>;
    return RV(std::forward<W>(value), std::forward<Bound>(bound));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::repeat_niebloid repeat{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_REPEAT_H_
