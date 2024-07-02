//
// Created by YongGyu Lee on 2024. 7. 1..
//

#ifndef PREVIEW_RANGES_VIEWS_LAZY_SPLIT_
#define PREVIEW_RANGES_VIEWS_LAZY_SPLIT_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__ranges/range_adaptor.h"
#include "preview/__ranges/range_value_t.h"
#include "preview/__ranges/viewable_range.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/single.h"
#include "preview/__ranges/views/lazy_split_view.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

struct lazy_split_niebloid {
  template<typename R, typename Pattern, std::enable_if_t<conjunction<
      viewable_range<R>,
      disjunction<
          viewable_range<Pattern>,
          std::is_same<range_value_t<R>, std::decay_t<Pattern>>
      >
  >::value, int> = 0>
  constexpr auto operator()(R&& r, Pattern&& pattern) const {
    return call(
        std::forward<R>(r),
        std::forward<Pattern>(pattern),
        std::is_same<range_value_t<R>, std::decay_t<Pattern>>{}
    );
  }

  template<typename Pattern>
  constexpr auto operator()(Pattern&& pattern) const {
    return range_adaptor<lazy_split_niebloid, std::decay_t<Pattern>>{std::forward<Pattern>(pattern)};
  }

 private:
  template<typename R, typename Pattern>
  constexpr auto call(R&& r, Pattern&& pattern, std::true_type /* range_value_t */) const {
    return lazy_split_view<all_t<R>, single_view<range_value_t<R>>>{std::forward<R>(r), std::forward<Pattern>(pattern)};
  }
  template<typename R, typename Pattern>
  constexpr auto call(R&& r, Pattern&& pattern, std::false_type /* range_value_t */) const {
    return lazy_split_view<all_t<R>, all_t<Pattern>>{std::forward<R>(r), std::forward<Pattern>(pattern)};
  }
};

} // namespace detail

namespace niebloid {
constexpr PREVIEW_INLINE_VARIABLE detail::lazy_split_niebloid lazy_split{};
} // namespace niebloid
using namespace niebloid;

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_LAZY_SPLIT_
