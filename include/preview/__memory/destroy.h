//
// Created by YongGyu Lee on 3/20/24.
//

#ifndef PREVIEW_MEMORY_DESTROY_H_
#define PREVIEW_MEMORY_DESTROY_H_

#include "preview/__core/constexpr.h"
#include "preview/__memory/addressof.h"
#include "preview/__memory/destroy_at.h"

namespace preview {

template<typename ForwardIt>
PREVIEW_CONSTEXPR_AFTER_CXX20 void destroy(ForwardIt first, ForwardIt last) {
  for (; first != last; ++first) {
    preview::destroy_at(preview::addressof(*first));
  }
}

} // namespace preview

#endif // PREVIEW_MEMORY_DESTROY_H_
