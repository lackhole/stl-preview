#ifndef PREVIEW_CORE_CONSTEXPR_H_
#define PREVIEW_CORE_CONSTEXPR_H_

#define PREVIEW_CONSTEXPR constexpr

#if __cplusplus >= 201402L
#define PREVIEW_CONSTEXPR_AFTER_CXX14 constexpr
#else
#define PREVIEW_CONSTEXPR_AFTER_CXX14
#endif

#if __cplusplus >= 201703L
#define PREVIEW_CONSTEXPR_AFTER_CXX17 constexpr
#else
#define PREVIEW_CONSTEXPR_AFTER_CXX17
#endif

#if __cplusplus >= 202002L
#define PREVIEW_CONSTEXPR_AFTER_CXX20 constexpr
#else
#define PREVIEW_CONSTEXPR_AFTER_CXX20
#endif

#if __cplusplus >= 202302L
#define PREVIEW_CONSTEXPR_AFTER_CXX23 constexpr
#else
#define PREVIEW_CONSTEXPR_AFTER_CXX23
#endif

#endif // PREVIEW_CORE_CONSTEXPR_H_
