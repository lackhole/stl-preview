//
// Created by Chi-Iroh on 09/11/2024.
//

#ifndef PREVIEW_BIT_ENDIAN_H_
#define PREVIEW_BIT_ENDIAN_H_

#include "preview/config.h"

#if PREVIEW_CXX_VERSION >= 20
  #include <bit>

  namespace preview {
      using endian = std::endian;
  }
#else
  namespace preview {
    enum class endian {
      little = PREVIEW_LITTLE_ENDIAN,
      big = PREVIEW_BIG_ENDIAN,
      native = PREVIEW_ENDIAN
    };
  };
#endif

#endif