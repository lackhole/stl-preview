#/**
# * Created by YongGyu Lee on 2024. 6. 20..
# */
#
#ifndef PREVIEW_TUPLE_SPECIALIZE_TUPLE_H_
#define PREVIEW_TUPLE_SPECIALIZE_TUPLE_H_
#
#include "preview/__core/android_version.h"
#
#define PREVIEW_STD_TUPLE_SIZE_STRUCT struct
#define PREVIEW_STD_TUPLE_ELEMENT_STRUCT struct
#
#if defined(__clang__)
#   if (__clang_major__ < 8) || (defined(PREVIEW_NDK_VERSION_MAJOR) && PREVIEW_NDK_VERSION_MAJOR < 20)
#       undef PREVIEW_STD_TUPLE_SIZE_STRUCT
#       define PREVIEW_STD_TUPLE_SIZE_STRUCT class
#   endif
#   if __clang_major__ < 9
#       undef PREVIEW_STD_TUPLE_ELEMENT_STRUCT
#       define PREVIEW_STD_TUPLE_ELEMENT_STRUCT class
#   endif
#endif
#
#include <tuple>

#define PREVIEW_SPECIALIZE_STD_TUPLE_SIZE(...) \
    PREVIEW_STD_TUPLE_SIZE_STRUCT std::tuple_size<__VA_ARGS__>

#define PREVIEW_SPECIALIZE_STD_TUPLE_ELEMENT(...) \
    PREVIEW_STD_TUPLE_ELEMENT_STRUCT std::tuple_element<__VA_ARGS__>

# endif // PREVIEW_TUPLE_SPECIALIZE_TUPLE_H_
