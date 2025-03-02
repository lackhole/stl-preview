//
// Created by yonggyulee on 2024. 9. 2.
//

#ifndef PREVIEW_ATOMIC_DETAIL_ATOMIC_FLOATING_POINT_H_
#define PREVIEW_ATOMIC_DETAIL_ATOMIC_FLOATING_POINT_H_

#include "preview/__atomic/detail/cxx20_atomic_base.h"

namespace preview {
namespace detail {

template<typename T>
struct atomic_floating_point : cxx20_atomic_base<T, is_cxx20_floating_point_atomic<std::atomic<T>>> {
 private:
  using base = cxx20_atomic_base<T, is_cxx20_floating_point_atomic<std::atomic<T>>>;

 public:
  using value_type = T;
  using difference_type = value_type;

  using base::base;
};

} // namespace detail
} // namespace preview

#endif // PREVIEW_ATOMIC_DETAIL_ATOMIC_FLOATING_POINT_H_
