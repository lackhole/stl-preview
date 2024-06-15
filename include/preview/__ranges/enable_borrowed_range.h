//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_ENABLE_BORROWED_RANGE_H_
#define PREVIEW_RANGES_ENABLE_BORROWED_RANGE_H_

#include <type_traits>

namespace preview {
namespace ranges {

template<typename R>
struct enable_borrowed_range : std::false_type {};

} // namespace preview
} // namespace ranges

#endif // PREVIEW_RANGES_ENABLE_BORROWED_RANGE_H_
