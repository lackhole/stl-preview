//
// Created by yonggyulee on 2024. 9. 4.
//

#ifndef PREVIEW_CONCEPTS_V2_FLOATING_POINT_H_
#define PREVIEW_CONCEPTS_V2_FLOATING_POINT_H_

#include <type_traits>

#include "preview/__concepts_v2/detail/concept_base.h"

namespace preview {
namespace concepts {
template<typename T> struct floating_point : concept_base<floating_point<T>, std::is_floating_point<T>> {};
}

template<typename T> PREVIEW_INLINE_VARIABLE constexpr concepts::floating_point<T> floating_point;

} // namespace preview

#endif // PREVIEW_CONCEPTS_V2_FLOATING_POINT_H_
