//
// Created by YongGyuLee on 2025-03-02.
//

#ifndef PREVIEW_ATOMIC_DETAIL_ATOMIC_MINMAX_H_
#define PREVIEW_ATOMIC_DETAIL_ATOMIC_MINMAX_H_

#include <algorithm>
#include <atomic>

#include "preview/__atomic/detail/cxx20_atomic_base.h"

namespace preview {
namespace detail {

// https://wg21.link/p0493r5

template<typename T>
constexpr T atomic_fetch_max_impl(cxx20_atomic_base<T>* obj, T x, std::memory_order order) noexcept {
  const std::memory_order order_adjusted = preview::detail::memory_order_drop_release(order);
  auto stored = (order_adjusted != order) ? obj->fetch_add(0, order) : obj->load(order_adjusted);
  while ((std::max)(x, stored) != stored) {
    if (obj->compare_exchange_weak(stored, x, order, order_adjusted))
      return stored;
  }
  return stored;
}

template<typename T>
constexpr T atomic_fetch_min_impl(cxx20_atomic_base<T>* obj, T x, std::memory_order order) noexcept {
  const std::memory_order order_adjusted = preview::detail::memory_order_drop_release(order);
  auto stored = (order_adjusted != order) ? obj->fetch_add(0, order) : obj->load(order_adjusted);
  while ((std::min)(x, stored) != stored) {
    if (obj->compare_exchange_weak(stored, x, order, order_adjusted))
      return stored;
  }
  return stored;
}

} // namespace detail
} // namespace preview

#endif // PREVIEW_ATOMIC_DETAIL_ATOMIC_MINMAX_H_
