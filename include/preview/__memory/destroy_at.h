//
// Created by YongGyu Lee on 3/20/24.
//

#ifndef PREVIEW_MEMORY_DESTROY_AT_H_
#define PREVIEW_MEMORY_DESTROY_AT_H_

#include <type_traits>

#include "preview/__core/constexpr.h"
#include "preview/__memory/addressof.h"

namespace preview {

template <typename T, std::enable_if_t<!std::is_array<T>::value, int> = 0>
PREVIEW_CONSTEXPR_AFTER_CXX20 void destroy_at(T* p) {
  p->~T();
}

template <typename T, std::enable_if_t<std::is_array<T>::value, int> = 0>
PREVIEW_CONSTEXPR_AFTER_CXX20 void destroy_at(T* p) {
  auto first = std::begin(*p);
  auto last = std::end(*p);

  for (; first != last; ++first) {
    preview::destroy_at(preview::addressof(*first));
  }
}

} // namespace preview

#endif // PREVIEW_MEMORY_DESTROY_AT_H_
