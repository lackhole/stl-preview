//
// Created by YongGyu Lee on 2/8/24.
//

#ifndef PREVIEW_RANGES_VIEWS_COMMON_H_
#define PREVIEW_RANGES_VIEWS_COMMON_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__ranges/common_range.h"
#include "preview/__ranges/range_adaptor_closure.h"
#include "preview/__ranges/viewable_range.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/common_view.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

struct common_adaptor_object : range_adaptor_closure<common_adaptor_object> {
 private:
  template<typename R>
  constexpr auto call(R&& r, std::true_type /* all and common */) const {
    return views::all(std::forward<R>(r));
  }
  template<typename R>
  constexpr auto call(R&& r, std::false_type /* all and common */) const {
    return common_view<all_t<R>>(std::forward<R>(r));
  }

 public:
  template<typename R, std::enable_if_t<viewable_range<R>::value, int> = 0>
  constexpr auto operator()(R&& r) const {
    return call(std::forward<R>(r), conjunction<is_invocable<all_adaptor_closure, R&&>, common_range<decltype((r))>>{});
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::common_adaptor_object common{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_COMMON_H_
