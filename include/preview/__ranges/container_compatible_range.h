//
// Created by yonggyulee on 03/02/2025
//

#ifndef PREVIEW_RANGES_CONTAINER_COMPATIBLE_RANGE_H_
#define PREVIEW_RANGES_CONTAINER_COMPATIBLE_RANGE_H_

#include "preview/__concepts/convertible_to.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/range_reference_t.h"

namespace preview {
namespace detail {

template<typename R, typename T, bool = ranges::input_range<R>::value /* true */>
struct container_compatible_range : convertible_to<ranges::range_reference_t<R>, T> {};
template<typename R, typename T>
struct container_compatible_range<R, T, false> : std::false_type {};

} // namespace detail
} // namespace preview

#endif // PREVIEW_RANGES_CONTAINER_COMPATIBLE_RANGE_H_
