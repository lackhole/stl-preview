//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_CONCEPTS_DERIVED_FROM_H_
#define PREVIEW_CONCEPTS_DERIVED_FROM_H_

#include <type_traits>

#include "preview/__type_traits/conjunction.h"

namespace preview {

template<typename Derived, typename Base>
struct derived_from
    : conjunction<
        std::is_base_of<Base, Derived>,
        std::is_convertible<const volatile Derived*, const volatile Base*>
      > {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_DERIVED_FROM_H_
