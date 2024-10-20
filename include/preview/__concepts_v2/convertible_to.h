//
// Created by yonggyulee on 2024. 10. 20.
//

#ifndef PREVIEW_INCLUDE_PREVIEW___CONCEPTS_V2_CONVERTIBLE_TO_H_
#define PREVIEW_INCLUDE_PREVIEW___CONCEPTS_V2_CONVERTIBLE_TO_H_

#include <type_traits>

#include "preview/__concepts_v2/concept_base.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_explicitly_convertible.h"

namespace preview {

#if defined(PREVIEW_USE_LEGACY_CONCEPT)

template<typename From, typename To>
struct convertible_to_c : concepts::concept_base<convertible_to_c<From, To>,
    conjunction<
        std::is_convertible<From, To>,
        is_explicitly_convertible<From, To>
    >
>{};

template<typename From, typename To>
PREVIEW_INLINE_VARIABLE convertible_to_c<From, To> convertible_to;

#else

template<typename From, typename To>
concept convertible_to =
    std::is_convertible_v<From, To> &&
    requires {
        static_cast<To>(std::declval<From>());
    };

#endif

} // namespace preview

#endif // PREVIEW_INCLUDE_PREVIEW___CONCEPTS_V2_CONVERTIBLE_TO_H_
