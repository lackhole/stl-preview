//
// Created by YongGyuLee on 2025-03-01.
//

#ifndef PREVIEW_ATOMIC_ATOMIC_WEAK_PTR_H_
#define PREVIEW_ATOMIC_ATOMIC_WEAK_PTR_H_

#include <memory>

#include "preview/__type_traits/bool_constant.h"

namespace preview {
namespace detail {

template<typename T>
struct atomic_weak_ptr {
  static_assert(always_false<T>::value, "Cannot be implemented without modifying the standard library");
};

} // namespace detail
} // namespace preview

#endif // PREVIEW_ATOMIC_ATOMIC_WEAK_PTR_H_
