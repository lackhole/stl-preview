//
// Created by yonggyulee on 2/3/24.
//

#ifndef PREVIEW_RANGES_VIEWS_SPLIT_H_
#define PREVIEW_RANGES_VIEWS_SPLIT_H_

#include <type_traits>
#include <utility>

#include "split_view.h"
#include "preview/__concepts/different_from.h"
#include "preview/__core/inline_variable.h"
#include "preview/__ranges/movable_box.h"
#include "preview/__ranges/range_adaptor_closure.h"
#include "preview/__ranges/range_value_t.h"
#include "preview/__ranges/viewable_range.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/single.h"
#include "preview/__ranges/views/split_view.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

template<typename Pattern>
class split_adapter_closure : public range_adaptor_closure<split_adapter_closure<Pattern>> {
  template<typename R, bool = ranges::detail::same_with_range_value<R, Pattern>::value /* true */>
  struct pattern_type_impl {
    using type = single_view<range_value_t<R>>;
  };
  template<typename R>
  struct pattern_type_impl<R, false> {
    using type = all_t<Pattern>;
  };

  template<typename R>
  using pattern_type = typename pattern_type_impl<R>::type;
  movable_box<Pattern> pattern_;

 public:
  template<typename T, std::enable_if_t<different_from<T, split_adapter_closure>::value, int> = 0>
  constexpr explicit split_adapter_closure(T&& pattern)
      : pattern_(std::forward<T>(pattern)) {}

  template<typename R, std::enable_if_t<range<R>::value, int> = 0>
  constexpr auto operator()(R&& r) & {
    return split_view<all_t<R>, pattern_type<R>>(std::forward<R>(r), *pattern_);
  }

  template<typename R, std::enable_if_t<range<R>::value, int> = 0>
  constexpr auto operator()(R&& r) const& {
    return split_view<all_t<R>, pattern_type<R>>(std::forward<R>(r), *pattern_);
  }

  template<typename R, std::enable_if_t<range<R>::value, int> = 0>
  constexpr auto operator()(R&& r) && {
    return split_view<all_t<R>, pattern_type<R>>(std::forward<R>(r), std::move(*pattern_));
  }

  template<typename R, std::enable_if_t<range<R>::value, int> = 0>
  constexpr auto operator()(R&& r) const && {
    return split_view<all_t<R>, pattern_type<R>>(std::forward<R>(r), std::move(*pattern_));
  }
};

struct split_niebloid {
  template<typename R, typename Pattern, std::enable_if_t<viewable_range<R>::value, int> = 0>
  constexpr auto operator()(R&& r, Pattern&& pattern) const {
    return preview::ranges::make_split_view(std::forward<R>(r), std::forward<Pattern>(pattern));
  }

  template<typename Pattern>
  constexpr split_adapter_closure<std::remove_reference_t<Pattern>> operator()(Pattern&& pattern) const {
    return split_adapter_closure<std::remove_reference_t<Pattern>>(std::forward<Pattern>(pattern));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::split_niebloid split{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_SPLIT_H_
