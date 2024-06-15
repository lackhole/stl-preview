//
// Created by yonggyulee on 2024/01/04.
//

#ifndef PREVIEW_CORE_NODISCARD_H_
#define PREVIEW_CORE_NODISCARD_H_

#if __cplusplus < 201703L
#  if defined(__GNUC__) && (__GNUC__ >= 4)
#    define PREVIEW_NODISCARD __attribute__ ((warn_unused_result))
#  elif defined(_MSC_VER) && (_MSC_VER >= 1700)
#    define PREVIEW_NODISCARD _Check_return_
#  else
#    define PREVIEW_NODISCARD
#  endif
#else
#  define PREVIEW_NODISCARD [[nodiscard]]
#endif

#endif // PREVIEW_CORE_CONSTEXPR_H_
