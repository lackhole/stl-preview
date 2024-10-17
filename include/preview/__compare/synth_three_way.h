//
// Created by yonggyulee on 2024. 10. 17.
//

#ifndef PREVIEW_COMPARE_SYNTH_THREE_WAY_H_
#define PREVIEW_COMPARE_SYNTH_THREE_WAY_H_

#include <type_traits>

#include "preview/__concepts/boolean_testable.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace detail {

template<typename T, typename U,
    bool = /* false */ conjunction_v<
        rel_ops::is_less_than_comparable<const T&, const U&>,
        rel_ops::is_less_equal_than_comparable<const U&, const T&>
    >
>
struct synth_three_way_possible : std::false_type {};

template<typename T, typename U>
struct synth_three_way_possible<T, U, true>
    : conjunction<
        boolean_testable<decltype(std::declval<const T&>() < std::declval<const U&>())>,
        boolean_testable<decltype(std::declval<const U&>() < std::declval<const T&>())>
    > {};

} // namespace detail
} // namespace preview

#endif // PREVIEW_COMPARE_SYNTH_THREE_WAY_H_
