//
// Created by yonggyulee on 2024. 7. 19.
//

#ifndef PREVIEW_ATOMIC_ATOMIC_WAIT_H_
#define PREVIEW_ATOMIC_ATOMIC_WAIT_H_

#include <atomic>
#include <cassert>

#include "preview/__atomic/atomic.h"

namespace preview {

template<typename T>
void atomic_wait(const atomic<T>* object, typename atomic<T>::value_type old) {
  object->wait(old);
}

template<typename T>
void atomic_wait(const volatile atomic<T>* object, typename atomic<T>::value_type old) {
  object->wait(old);
}

template<typename T>
void atomic_wait_explicit(const atomic<T>* object, typename atomic<T>::value_type old, std::memory_order order) {
  assert(order != std::memory_order_relaxed);
  assert(order != std::memory_order_acq_rel);

  object->wait(old, order);
}

template<typename T>
void atomic_wait_explicit(const volatile atomic<T>* object, typename atomic<T>::value_type old, std::memory_order order) {
  assert(order != std::memory_order_relaxed);
  assert(order != std::memory_order_acq_rel);

  object->wait(old, order);
}

} // namespace preview

#endif // PREVIEW_ATOMIC_ATOMIC_WAIT_H_
