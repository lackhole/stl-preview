#ifndef PREVIEW_CONCEPTS_V2_DESTRUCTIBLE_H_
#define PREVIEW_CONCEPTS_V2_DESTRUCTIBLE_H_

#include <type_traits>

#include "preview/__concepts_v2/concept_base.h"

namespace preview {

#if defined(PREVIEW_USE_LEGACY_CONCEPT)

template<typename T>
struct destructible_c : concepts::concept_base<destructible_c<T>, std::is_nothrow_destructible<T>> {};

template<typename T> 
PREVIEW_INLINE_VARIABLE constexpr destructible_c<T> destructible;

#else

template<typename T>
concept destructible = std::is_nothrow_destructible_v<T>;

#endif

} // namespace preview

#endif // PREVIEW_CONCEPTS_V2_DESTRUCTIBLE_H_
