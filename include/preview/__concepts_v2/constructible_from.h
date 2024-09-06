#ifndef PREVIEW_CONCEPTS_V2_CONSTRUCTIBLE_FROM_H_
#define PREVIEW_CONCEPTS_V2_CONSTRUCTIBLE_FROM_H_

#include <type_traits>

#include "preview/__concepts_v2/detail/concept_base.h"
#include "preview/__concepts_v2/destructible.h"

namespace preview {
namespace concepts {

template<typename T, typename... Args>
struct constructible_from : concept_base<constructible_from<T, Args...>, decltype(
    destructible<T>{} && std::is_constructible<T, Args...>{}
)> {};

} // namespace concepts

template<typename T, typename... Args>
PREVIEW_INLINE_VARIABLE constexpr concepts::constructible_from<T, Args...> constructible_from;

} // namespace preview


#endif // PREVIEW_CONCEPTS_V2_CONSTRUCTIBLE_FROM_H_
