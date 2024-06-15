//
// Created by YongGyu Lee on 2/8/24.
//

#ifndef PREVIEW_RANGES_DETAIL_NOT_INCOMPLETE_ARRAY_H_
#define PREVIEW_RANGES_DETAIL_NOT_INCOMPLETE_ARRAY_H_

#include <type_traits>

#include "preview/__type_traits/is_complete.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace detail {

template<typename T, bool = std::is_array<remove_cvref_t<T>>::value /* false */>
struct not_incomplete_array : std::true_type {};

template<typename T>
struct not_incomplete_array<T, true>
    : is_complete<std::remove_all_extents_t<std::remove_reference_t<T>>> {};

} // namespace detail
} // namespace preview

#endif // PREVIEW_RANGES_DETAIL_NOT_INCOMPLETE_ARRAY_H_
