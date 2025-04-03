#ifndef PREVIEW_CORE_CXX_VERSION_H_
#define PREVIEW_CORE_CXX_VERSION_H_

#ifndef __cplusplus
#error "__cplusplus is not defined"
#endif

#ifndef PREVIEW_CXX_VERSION
#  if __cplusplus >= 202302L
#    define PREVIEW_CXX_VERSION 23
#  elif __cplusplus >= 202002L
#    define PREVIEW_CXX_VERSION 20
#  elif __cplusplus >= 201703L
#    define PREVIEW_CXX_VERSION 17
#  elif __cplusplus >= 201402L
#    define PREVIEW_CXX_VERSION 14
#  else
#    error "C++14 is required"
#  endif
#endif

#define PREVIEW_BEFORE_CXX17(...)
#define PREVIEW_BEFORE_CXX20(...)
#define PREVIEW_BEFORE_CXX23(...)

//#define PREVIEW_SINCE_CXX14_BEFORE_CXX17(...)
//#define PREVIEW_SINCE_CXX14_BEFORE_CXX20(...)
//#define PREVIEW_SINCE_CXX14_BEFORE_CXX23(...)
//#define PREVIEW_SINCE_CXX17_BEFORE_CXX20(...)
//#define PREVIEW_SINCE_CXX17_BEFORE_CXX23(...)
//#define PREVIEW_SINCE_CXX20_BEFORE_CXX23(...)

#if PREVIEW_CXX_VERSION < 17
#undef PREVIEW_BEFORE_CXX17
#define PREVIEW_BEFORE_CXX17(...) __VA_ARGS__
#endif

#if PREVIEW_CXX_VERSION < 20
#undef PREVIEW_BEFORE_CXX20
#define PREVIEW_BEFORE_CXX20(...) __VA_ARGS__
#endif

#if PREVIEW_CXX_VERSION < 23
#undef PREVIEW_BEFORE_CXX23
#define PREVIEW_BEFORE_CXX23(...) __VA_ARGS__
#endif

#endif // PREVIEW_CORE_CXX_VERSION_H_
