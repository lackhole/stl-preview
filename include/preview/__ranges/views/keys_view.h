//
// Created by yonggyulee on 1/26/24.
//

#ifndef PREVIEW_RANGES_VIEWS_KEYS_VIEW_H_
#define PREVIEW_RANGES_VIEWS_KEYS_VIEW_H_

#include "preview/__ranges/views/elements.h"
#include "preview/__ranges/views/elements_view.h"

namespace preview {
namespace ranges {

template<typename R>
using keys_view = elements_view<R, 0>;

namespace views {

PREVIEW_INLINE_VARIABLE constexpr detail::elements_adaptor_object<0> keys{};

} // namespace views

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_KEYS_VIEW_H_
