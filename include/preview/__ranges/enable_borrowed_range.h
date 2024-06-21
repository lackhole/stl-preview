//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_ENABLE_BORROWED_RANGE_H_
#define PREVIEW_RANGES_ENABLE_BORROWED_RANGE_H_
#
#include <type_traits>
#
#include "preview/core.h"

#if PREVIEW_CXX_VERSION >= 20
#include <ranges>
#endif

namespace preview {
namespace ranges {

#if PREVIEW_CXX_VERSION < 20
template<typename R>
PREVIEW_INLINE_VARIABLE constexpr bool enable_borrowed_range = false;
#else
template<typename R>
PREVIEW_INLINE_VARIABLE constexpr bool enable_borrowed_range = std::ranges::enable_borrowed_range<R>;
#endif

template<typename R>
struct enable_borrowed_range_t : std::integral_constant<bool, enable_borrowed_range<R>> {};

} // namespace preview
} // namespace ranges

#if PREVIEW_CXX_VERSION < 20
#   define PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(...) \
        PREVIEW_INLINE_VARIABLE constexpr bool preview::ranges::enable_borrowed_range<__VA_ARGS__>
#else
#   define PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(...) \
        inline constexpr bool std::ranges::enable_borrowed_range<__VA_ARGS__>
#endif

#if 17 <= PREVIEW_CXX_VERSION && PREVIEW_CXX_VERSION < 20
#include <string_view>

template<typename CharT, typename Traits>
PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(std::basic_string_view<CharT, Traits>) = true;
#endif

#endif // PREVIEW_RANGES_ENABLE_BORROWED_RANGE_H_
