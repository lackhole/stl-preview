//
// Created by yonggyulee on 2024. 9. 4.
//

#ifndef PREVIEW_CONCEPTS_V2_FLOATING_POINT_H_
#define PREVIEW_CONCEPTS_V2_FLOATING_POINT_H_

#include <type_traits>

#include "preview/__concepts_v2/concept_base.h"

namespace preview {

#if defined(PREVIEW_USE_LEGACY_CONCEPT)

template<typename T> struct floating_point_c : concepts::concept_base<floating_point_c<T>, std::is_floating_point<T>> {};
template<typename T> PREVIEW_INLINE_VARIABLE constexpr floating_point_c<T> floating_point;

#else

template<typename T>
concept floating_point = std::is_floating_point_v<T>;

#endif

} // namespace preview

#endif // PREVIEW_CONCEPTS_V2_FLOATING_POINT_H_
