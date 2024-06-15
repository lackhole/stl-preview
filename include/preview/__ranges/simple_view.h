//
// Created by yonggyulee on 2024/01/03.
//

#ifndef PREVIEW_RANGES_SIMPLE_VIEW_H_
#define PREVIEW_RANGES_SIMPLE_VIEW_H_

#include <type_traits>

#include "preview/__concepts/same_as.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/sentinel_t.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/view.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {

template<typename R, bool = conjunction<view<R>, range<const R>>::value /* true */>
struct simple_view
    : conjunction<
          same_as<iterator_t<R>, iterator_t<const R>>,
          same_as<sentinel_t<R>, sentinel_t<const R>>
      > {};

template<typename R>
struct simple_view<R, false> : std::false_type {};

} // namespace preview
} // namespace ranges

#endif // PREVIEW_RANGES_SIMPLE_VIEW_H_
