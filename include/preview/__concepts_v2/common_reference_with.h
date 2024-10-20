//
// Created by yonggyulee on 2024. 10. 20.
//

#ifndef PREVIEW_INCLUDE_PREVIEW___CONCEPTS_V2_COMMON_REFERENCE_WITH_H_
#define PREVIEW_INCLUDE_PREVIEW___CONCEPTS_V2_COMMON_REFERENCE_WITH_H_

#include "preview/__concepts_v2/concept_base.h"
#include "preview/__concepts_v2/convertible_to.h"
#include "preview/__concepts_v2/same_as.h"
#include "preview/__type_traits/common_reference.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {

#if defined(PREVIEW_USE_LEGACY_CONCEPT)

namespace detail {

template<typename T, typename U>
struct common_reference_with_helper : concepts::concept_base<common_reference_with_helper<T, U>,
    has_typename_type<common_reference<T, U>>
> {};

} // namespace detail

template<typename T, typename U>
struct common_reference_with_c : concepts::concept_base<common_reference_with_c<T, U>, decltype(
    detail::common_reference_with_helper<T, U>{} && detail::common_reference_with_helper<U, T>{}
)> {};

template<typename T, typename U>
PREVIEW_INLINE_VARIABLE constexpr common_reference_with_c<T, U> common_reference_with;

#else

#endif

} // namespace preview

#endif // PREVIEW_INCLUDE_PREVIEW___CONCEPTS_V2_COMMON_REFERENCE_WITH_H_
