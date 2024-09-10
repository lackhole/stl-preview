//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_ENABLE_BORROWED_RANGE_H_
#define PREVIEW_RANGES_ENABLE_BORROWED_RANGE_H_
#
#include <type_traits>
#
#include "preview/core.h"
#include "preview/__ranges/detail/have_cxx20_ranges.h"

#if PREVIEW_HAVE_CXX20_RANGES
#include <ranges>
#endif

namespace preview {
namespace ranges {

#if PREVIEW_HAVE_CXX20_RANGES
template<typename R>
PREVIEW_INLINE_VARIABLE constexpr bool enable_borrowed_range = std::ranges::enable_borrowed_range<R>;
#else
template<typename R>
PREVIEW_INLINE_VARIABLE constexpr bool enable_borrowed_range = false;
#endif

template<typename R>
struct enable_borrowed_range_t : std::integral_constant<bool, enable_borrowed_range<R>> {};

} // namespace preview
} // namespace ranges


#if defined(_MSC_VER) && _MSC_VER < 1920
#define PREVIEW_RANGES_NS ::preview::ranges::
#else
#define PREVIEW_RANGES_NS preview::ranges::
#endif

#if PREVIEW_HAVE_CXX20_RANGES
#   define PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(...) \
        inline constexpr bool std::ranges::enable_borrowed_range<__VA_ARGS__>
#else
#   define PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(...) \
        PREVIEW_INLINE_VARIABLE constexpr bool PREVIEW_RANGES_NS enable_borrowed_range<__VA_ARGS__>
#endif

#if 17 <= PREVIEW_CXX_VERSION && PREVIEW_CXX_VERSION < 20
#include <string_view>

template<typename CharT, typename Traits>
PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(std::basic_string_view<CharT, Traits>) = true;
#endif

#endif // PREVIEW_RANGES_ENABLE_BORROWED_RANGE_H_
