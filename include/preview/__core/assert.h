//
// Created by YongGyu Lee on 2024. 6. 15..
//

#ifndef PREVIEW_INCLUDE_PREVIEW___CORE_ASSERT_H_
#define PREVIEW_INCLUDE_PREVIEW___CORE_ASSERT_H_

#include <cassert>

#define PREVIEW_DEBUG_ASSERT_IMPL_1(expr) \
    assert((expr))
#define PREVIEW_DEBUG_ASSERT_IMPL_2(expr, msg) \
    assert(((void)msg, (expr)))
#define PREVIEW_DEBUG_ASSERT_IMPL_N(_1, _2, N, ...) \
    PREVIEW_DEBUG_ASSERT_IMPL_##N

# ifndef NDEBUG
#   define PREVIEW_DEBUG_ASSERT(...) PREVIEW_DEBUG_ASSERT_IMPL_N(__VA_ARGS__, 2, 1) (__VA_ARGS__)
# else
#   define PREVIEW_DEBUG_ASSERT(...)
# endif

#endif //PREVIEW_INCLUDE_PREVIEW___CORE_ASSERT_H_
