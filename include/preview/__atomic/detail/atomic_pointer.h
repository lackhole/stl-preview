//
// Created by yonggyulee on 2024. 9. 2.
//

#ifndef PREVIEW_ATOMIC_DETAIL_ATOMIC_POINTER_H_
#define PREVIEW_ATOMIC_DETAIL_ATOMIC_POINTER_H_

#include "preview/__atomic/detail/cxx20_atomic_base.h"

namespace preview {
namespace detail {

template<typename T>
struct atomic_pointer : public cxx20_atomic_base<T> {

  // TODO: Implement fetch_max
//  T fetch_max(T operand, std::memory_order order = std::memory_order_seq_cst) noexcept {}
//  T fetch_max(T operand, std::memory_order order = std::memory_order_seq_cst) volatile noexcept {}
};

} // namespace detail
} // namespace preview

#endif // PREVIEW_ATOMIC_DETAIL_ATOMIC_POINTER_H_
