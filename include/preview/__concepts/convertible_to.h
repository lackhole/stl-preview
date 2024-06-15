//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_CONCEPTS_CONVERTIBLE_TO_H_
#define PREVIEW_CONCEPTS_CONVERTIBLE_TO_H_

#include <type_traits>

#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_explicitly_convertible.h"

namespace preview {

template<typename From, typename To>
struct convertible_to :
    conjunction<
      std::is_convertible<From, To>,
      is_explicitly_convertible<From, To>
    > {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_CONVERTIBLE_TO_H_
