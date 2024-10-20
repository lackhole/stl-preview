#ifndef PREVIEW_CONCEPTS_V2_CONSTRUCTIBLE_FROM_H_
#define PREVIEW_CONCEPTS_V2_CONSTRUCTIBLE_FROM_H_

#include <type_traits>

#include "preview/__concepts_v2/concept_base.h"
#include "preview/__concepts_v2/destructible.h"

namespace preview {

#if defined(PREVIEW_USE_LEGACY_CONCEPT)

template<typename T, typename... Args>
struct constructible_from_c : concepts::concept_base<constructible_from_c<T, Args...>, decltype(
    destructible<T> && std::is_constructible<T, Args...>{}
)> {};

template<typename T, typename... Args>
PREVIEW_INLINE_VARIABLE constexpr constructible_from_c<T, Args...> constructible_from{};

#else

template<typename T, typename... Args>
concept constructible_from = destructible<T> && std::is_constructible_v<T, Args...>;

#endif

} // namespace preview


#endif // PREVIEW_CONCEPTS_V2_CONSTRUCTIBLE_FROM_H_
