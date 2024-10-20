//
// Created by yonggyulee on 2024. 10. 20.
//

#ifndef PREVIEW_CONCEPTS_V2_SAME_AS_H_
#define PREVIEW_CONCEPTS_V2_SAME_AS_H_

#include <type_traits>

#include "preview/__concepts_v2/concept_base.h"

namespace preview {

#if defined(PREVIEW_USE_LEGACY_CONCEPT)

template<typename T, typename U>
struct same_as_c : concepts::concept_base<same_as_c<T, U>, std::is_same<T, U>> {};
template<typename T, typename U>
PREVIEW_INLINE_VARIABLE constexpr same_as_c<T, U> same_as{};

#else

namespace detail {

template<typename T, typename U>
concept same_as_impl = std::is_same_v<T, U>;

} // namespace detail

template<typename T, typename U>
concept same_as = detail::same_as_impl<T, U> && detail::same_as_impl<U, T>;

#endif

} // namespace preview

#endif // PREVIEW_CONCEPTS_V2_SAME_AS_H_
