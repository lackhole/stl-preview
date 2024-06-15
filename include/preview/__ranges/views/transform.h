//
// Created by yonggyulee on 2023/01/12.
//

#ifndef PREVIEW_RANGES_VIEWS_TRANSFORM_H_
#define PREVIEW_RANGES_VIEWS_TRANSFORM_H_

#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__concepts/copy_constructible.h"
#include "preview/__concepts/invocable.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/range_adaptor.h"
#include "preview/__ranges/viewable_range.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/transform_view.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

struct transform_niebloid {
 private:
  template<typename R, typename F, bool = conjunction<input_range<R>, copy_constructible<F>>::value /* false */>
  struct check_range : std::false_type {};
  template<typename R, typename F>
  struct check_range<R, F, true>
      : conjunction<
            view<R>,
            std::is_object<F>,
            regular_invocable<F&, range_reference_t<R>>
        >{};

 public:
  template<typename R, typename F, std::enable_if_t<conjunction<
      viewable_range<R>,
      check_range<all_t<R>, std::decay_t<F>>
  >::value, int> = 0>
  constexpr auto operator()(R&& r, F&& fun) const {
    return transform_view<all_t<R>, std::decay_t<F>>(std::forward<R>(r), std::forward<F>(fun));
  }

  template<typename F, std::enable_if_t<conjunction<
      copy_constructible<std::decay_t<F>>,
      std::is_object<std::decay_t<F>>
  >::value, int> = 0>
  constexpr auto operator()(F&& fun) const {
    return range_adaptor<transform_niebloid, std::decay_t<F>>(std::forward<F>(fun));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::transform_niebloid transform{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_TRANSFORM_H_
