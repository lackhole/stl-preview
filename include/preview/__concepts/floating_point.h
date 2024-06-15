//
// Created by yonggyulee on 2023/12/27.
//

#ifndef PREVIEW_CONCEPTS_FLOATING_POINT_H_
#define PREVIEW_CONCEPTS_FLOATING_POINT_H_

#include <type_traits>

namespace preview {

template<typename T>
struct floating_point : std::is_floating_point<T> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_FLOATING_POINT_H_
