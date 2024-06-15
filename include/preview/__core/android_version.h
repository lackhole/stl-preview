//
// Created by YongGyu Lee on 2024. 6. 16..
//

# ifndef PREVIEW_CORE_ANDROID_VERSION_H_
# define PREVIEW_CORE_ANDROID_VERSION_H_
#
# /* Set PREVIEW_ANDROID to 1 if Android, 0 otherwise */
# if (defined(ANDROID) || defined(__ANDROID__))
#   define PREVIEW_ANDROID 1
# else
#   define PREVIEW_ANDROID 0
# endif
#
# /* Set PREVIEW_NDK_VERSION_MAJOR to __NDK_MAJOR__ iif defined */
# if PREVIEW_ANDROID
# include <android/api-level.h>
#   ifdef __ANDROID_API_O_MR1__
#     include <android/ndk-version.h>
#     ifdef __NDK_MAJOR__
#       define PREVIEW_NDK_VERSION_MAJOR   __NDK_MAJOR__
#     endif
#   endif
# endif
#
# endif // PREVIEW_CORE_ANDROID_VERSION_H_
