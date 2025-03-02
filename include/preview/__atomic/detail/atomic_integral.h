//
// Created by yonggyulee on 2024. 9. 2.
//

#ifndef PREVIEW_ATOMIC_DETAIL_ATOMIC_INTEGRAL_H_
#define PREVIEW_ATOMIC_DETAIL_ATOMIC_INTEGRAL_H_

#include <type_traits>

#include "preview/__atomic/detail/cxx20_atomic_base.h"
#include "preview/__atomic/detail/atomic_minmax.h"

namespace preview {
namespace detail {

template<typename T>
struct atomic_integral : cxx20_atomic_base<T> {
 private:
  using base = cxx20_atomic_base<T>;

 public:
  using value_type = typename base::value_type;
  using difference_type = value_type;

  using base::base;

  template<bool B = has_member_function_fetch_max<base>::value, std::enable_if_t<!B, int> = 0>
  constexpr T fetch_max(T x, std::memory_order order = std::memory_order_seq_cst) noexcept {
    return preview::detail::atomic_fetch_max_impl(this, x, order);
  }

  template<bool B = has_member_function_fetch_min<base>::value, std::enable_if_t<!B, int> = 0>
  constexpr T fetch_min(T x, std::memory_order order = std::memory_order_seq_cst) noexcept {
    return preview::detail::atomic_fetch_min_impl(this, x, order);
  }
};

} // namespace detail
} // namespace preview

#endif // PREVIEW_ATOMIC_DETAIL_ATOMIC_INTEGRAL_H_
