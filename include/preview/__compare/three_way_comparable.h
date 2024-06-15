//
// Created by YongGyu Lee on 4/11/24.
//

#ifndef PREVIEW_COMPARE_THREE_WAY_COMPARABLE_H_
#define PREVIEW_COMPARE_THREE_WAY_COMPARABLE_H_

#include "preview/__concepts/partially_ordered_with.h"
#include "preview/__concepts/weakly_equality_comparable_with.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {

template<typename T>
struct unstable_three_way_comparable
    : conjunction<
        weakly_equality_comparable_with<T, T>,
        partially_ordered_with<T, T>
    > {};

} // namespace preview

#endif // PREVIEW_COMPARE_THREE_WAY_COMPARABLE_H_
