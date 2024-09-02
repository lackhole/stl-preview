//
// Created by yonggyulee on 2024. 9. 2.
//

#ifndef PREVIEW_ATOMIC_DETAIL_ATOMIC_INTEGRAL_H_
#define PREVIEW_ATOMIC_DETAIL_ATOMIC_INTEGRAL_H_

#include "boost/atomic.hpp"

#include "preview/__atomic/detail/atomic_base.h"

namespace preview {
namespace detail {

template<typename T>
class atomic_integral : public atomic_base<T> {
 public:
  using atomic_base<T>::atomic_base;

  // TODO: Implement fetch_max
//  T fetch_max(T operand, std::memory_order order = std::memory_order_seq_cst) noexcept {}
//  T fetch_max(T operand, std::memory_order order = std::memory_order_seq_cst) volatile noexcept {}

};

} // namespace detail
} // namespace preview

#endif // PREVIEW_ATOMIC_DETAIL_ATOMIC_INTEGRAL_H_
