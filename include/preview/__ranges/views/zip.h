//
// Created by YongGyu Lee on 4/9/24.
//

#ifndef PREVIEW_RANGES_VIEWS_ZIP_H_
#define PREVIEW_RANGES_VIEWS_ZIP_H_

#include <tuple>
#include <type_traits>

#include "preview/__core/decay_copy.h"
#include "preview/__core/inline_variable.h"
#include "preview/__ranges/viewable_range.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/empty_view.h"
#include "preview/__ranges/views/zip_view.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

struct zip_niebloid {
  constexpr auto operator()() const {
    return preview_decay_copy(views::empty<std::tuple<>>);
  }

  template<typename... Rs, std::enable_if_t<conjunction<
      viewable_range<Rs>...
  >::value, int> = 0>
  constexpr auto operator()(Rs&&... rs) const {
    return ranges::zip_view<views::all_t<decltype((rs))>...>(rs...);
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::zip_niebloid zip{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_ZIP_H_
