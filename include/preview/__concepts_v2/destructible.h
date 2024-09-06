#ifndef PREVIEW_CONCEPTS_V2_DESTRUCTIBLE_H_
#define PREVIEW_CONCEPTS_V2_DESTRUCTIBLE_H_

#include "preview/__concepts_v2/detail/concept_base.h"

namespace preview {
namespace concepts {

template<typename T>
struct destructible : concept_base<destructible<T>, std::is_nothrow_destructible<T>> {};

} // namespace concepts

template<typename T> 
PREVIEW_INLINE_VARIABLE constexpr concepts::destructible<T> destructible;

} // namespace preview

#endif // PREVIEW_CONCEPTS_V2_DESTRUCTIBLE_H_
