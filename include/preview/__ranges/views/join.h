//
// Created by yonggyulee on 1/31/24.
//

#ifndef PREVIEW_RANGES_VIEWS_JOIN_H_
#define PREVIEW_RANGES_VIEWS_JOIN_H_

#include <type_traits>
#include <utility>

#include "preview/__ranges/range.h"
#include "preview/__ranges/range_adaptor_closure.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/join_view.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

class join_adaptor_object : public range_adaptor_closure<join_adaptor_object> {
 public:
  template<typename R, std::enable_if_t<range<R>::value, int> = 0>
  constexpr auto operator()(R&& r) const {
    return join_view<all_t<decltype((r))>>{std::forward<R>(r)};
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::join_adaptor_object join{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_JOIN_H_
