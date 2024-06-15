//
// Created by yonggyulee on 1/26/24.
//

#ifndef PREVIEW_RANGES_VIEWS_VALUES_VIEW_H_
#define PREVIEW_RANGES_VIEWS_VALUES_VIEW_H_

#include "preview/__ranges/views/elements.h"
#include "preview/__ranges/views/elements_view.h"

namespace preview {
namespace ranges {

template<typename R>
using values_view = elements_view<R, 1>;

namespace views {

PREVIEW_INLINE_VARIABLE constexpr detail::elements_adaptor_object<1> values{};

} // namespace views

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_VALUES_VIEW_H_
