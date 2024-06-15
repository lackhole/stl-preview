//
// Created by yonggyulee on 2024/01/22.
//

#ifndef PREVIEW_RANGES_VIEWS_ELEMENTS_H_
#define PREVIEW_RANGES_VIEWS_ELEMENTS_H_

#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__ranges/range_adaptor_closure.h"
#include "preview/__ranges/viewable_range.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/elements_view.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

template<std::size_t N>
class elements_adaptor_object : public range_adaptor_closure<elements_adaptor_object<N>> {
 public:
  constexpr elements_adaptor_object() = default;

  template<typename R, std::enable_if_t<viewable_range<R>::value, int> = 0>
  constexpr auto operator()(R&& r) const {
    return elements_view<all_t<R>, N>(std::forward<R>(r));
  }
};

} // namespace detail

template<std::size_t N>
PREVIEW_INLINE_VARIABLE constexpr detail::elements_adaptor_object<N> elements{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_ELEMENTS_H_
