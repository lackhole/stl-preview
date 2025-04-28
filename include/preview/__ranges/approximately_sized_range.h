//
// Created by yonggyulee on 28/04/2025
//

#ifndef PREVIEW_RANGES_APPROXIMATELY_SIZED_RANGE_H_
#define PREVIEW_RANGES_APPROXIMATELY_SIZED_RANGE_H_

#include "preview/__concepts/invocable.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/reserve_hint.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {

template<typename T>
struct approximately_sized_range : conjunction<
    ranges::range<T>,
    invocable<decltype(ranges::reserve_hint, std::declval<T&>())>
> {};

} // namespace ranges
} // namespace preview 

#endif // PREVIEW_RANGES_APPROXIMATELY_SIZED_RANGE_H_
