//
// Created by yonggyulee on 2024. 10. 20.
//

#ifndef PREVIEW_CONCEPTS_V2_DERIVED_FROM_H_
#define PREVIEW_CONCEPTS_V2_DERIVED_FROM_H_

#include <type_traits>

#include "preview/__concepts_v2/concept_base.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {

#if defined(PREVIEW_USE_LEGACY_CONCEPT)

template<typename Derived, typename Base>
struct derived_from_c : concepts::concept_base<derived_from_c<Derived, Base>,
    conjunction<
        std::is_base_of<Base, Derived>,
        std::is_convertible<const volatile Derived*, const volatile Base*>
    >
> {};

template<typename Derived, typename Base>
PREVIEW_INLINE_VARIABLE constexpr derived_from_c<Derived, Base> derived_from;

#else

template<typename Derived, typename Base>
concept derived_from =
    std::is_base_of_v<Base, Derived> &&
    std::is_convertible_v<const volatile Derived*, const volatile Base*>;

#endif

} // namespace preview

#endif // PREVIEW_CONCEPTS_V2_DERIVED_FROM_H_
