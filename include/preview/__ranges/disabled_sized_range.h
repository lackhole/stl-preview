//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_DISABLED_SIZED_RANGE_H_
#define PREVIEW_RANGES_DISABLED_SIZED_RANGE_H_

#include <type_traits>

namespace preview {
namespace ranges {

template<typename T>
struct disabled_sized_range : std::false_type {};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_DISABLED_SIZED_RANGE_H_
