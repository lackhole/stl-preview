//
// Created by yonggyulee on 2024. 9. 2.
//

#ifndef PREVIEW_ATOMIC_DETAIL_ATOMIC_FLOATING_POINT_H_
#define PREVIEW_ATOMIC_DETAIL_ATOMIC_FLOATING_POINT_H_

#include "preview/__atomic/detail/atomic_base.h"

namespace preview {
namespace detail {

template<typename T>
struct atomic_floating_point : atomic_base<T> {
  using value_type      = T;
  using difference_type = value_type;

  using atomic_base<T>::atomic_base;
};

} // namespace detail
} // namespace preview

#endif // PREVIEW_ATOMIC_DETAIL_ATOMIC_FLOATING_POINT_H_
