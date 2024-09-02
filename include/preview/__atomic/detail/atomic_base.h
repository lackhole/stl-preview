//
// Created by yonggyulee on 2024. 9. 2.
//

#ifndef PREVIEW_ATOMIC_DETAIL_ATOMIC_BASE_H_
#define PREVIEW_ATOMIC_DETAIL_ATOMIC_BASE_H_

#include <atomic>
#include <type_traits>

#include "boost/atomic.hpp"

#include "preview/__atomic/detail/cxx20_atomic.h"

namespace preview {
namespace detail {
namespace atomics {

template<typename T, typename = void>
struct has_member_a : std::false_type {};
template<typename T>
struct has_member_a<T, std::void_t<decltype(std::declval<T>().__a_)>> : std::true_type {};

} // namespace atomics

template<typename T>
class cxx20_atomic_base : public std::atomic<T> {
 public:
  using value_type      = T;
  using difference_type = value_type;

 private:
  static constexpr std::size_t storage_size = boost::atomics::detail::storage_size_of<value_type>::value;
  static constexpr bool is_signed = std::is_signed<value_type>::value;
  static constexpr bool interprocess = false;

  using boost_core_operations = boost::atomics::detail::core_operations<storage_size, is_signed, interprocess>;
  using boost_wait_operations = boost::atomics::detail::wait_operations<boost_core_operations>;

  using std_atomic_base = std::atomic<T>;

 public:
  using std_atomic_base::std_atomic_base;

  void wait(T old, std::memory_order order = std::memory_order_seq_cst) const noexcept {
    assert(order != std::memory_order_release);
    assert(order != std::memory_order_acq_rel);

    this->__a_;
  }
};

template<typename T>
using cxx20_atomic_base_t = std::conditional_t<
    has_wait_and_notify<std::atomic<T>>::value, std::atomic<T>,
    cxx20_atomic_base<T>
>;

template<typename T>
class atomic_base : public cxx20_atomic_base_t<T> {
 public:
  using cxx20_atomic_base_t<T>::cxx20_atomic_base_t;
};

} // namespace detail
} // namespace preview

#endif // PREVIEW_ATOMIC_DETAIL_ATOMIC_BASE_H_
