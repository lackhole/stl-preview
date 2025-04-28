//
// Created by yonggyulee on 23/04/2025
//

#ifndef PREVIEW_MEMORY_DETAIL_VOIDIFY_H_
#define PREVIEW_MEMORY_DETAIL_VOIDIFY_H_

#include "preview/__memory/addressof.h"

namespace preview {
namespace detail {

template<class T>
constexpr void* voidify(T& obj) noexcept {
  return preview::addressof(obj);
}

} // namespace detail
} // namespace preview 

#endif // PREVIEW_MEMORY_DETAIL_VOIDIFY_H_
