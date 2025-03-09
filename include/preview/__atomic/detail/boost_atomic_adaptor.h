//
// Created by YongGyuLee on 2025-03-02.
//

#ifndef PREVIEW_ATOMIC_DETAIL_BOOST_ATOMIC_ADAPTOR_H_
#define PREVIEW_ATOMIC_DETAIL_BOOST_ATOMIC_ADAPTOR_H_

#include <atomic>
#include <type_traits>

#include "boost/atomic.hpp"

#include "preview/__type_traits/has_typename_difference_type.h"
#include "preview/__type_traits/void_t.h"
#include "preview/__utility/unreachable.h"

namespace preview {
namespace detail {

// BOOST_FORCEINLINE constexpr boost::memory_order to_boost_order_impl(std::memory_order order, std::true_type) noexcept {
//   static constexpr boost::memory_order boost_order[] = {
//       boost::memory_order_relaxed,
//       boost::memory_order_consume,
//       boost::memory_order_acquire,
//       boost::memory_order_release,
//       boost::memory_order_acq_rel,
//       boost::memory_order_seq_cst
//   };
//   return boost_order[to_underlying(order)];
// }

BOOST_FORCEINLINE constexpr boost::memory_order to_boost_order(std::memory_order order) noexcept {
  switch (order) {
    case std::memory_order_relaxed:
      return boost::memory_order_relaxed;
    case std::memory_order_consume:
      return boost::memory_order_consume;
    case std::memory_order_acquire:
      return boost::memory_order_acquire;
    case std::memory_order_release:
      return boost::memory_order_release;
    case std::memory_order_acq_rel:
      return boost::memory_order_acq_rel;
    case std::memory_order_seq_cst:
      return boost::memory_order_seq_cst;
  }
  preview::unreachable();
}

template<typename T, typename Base>
struct boost_atomic_adaptor_fetch_bitwise_op : Base {
private:
  using base = Base;

  using base::fetch_and;
  using base::fetch_or;
  using base::fetch_xor;

public:
  using base::base;

  BOOST_FORCEINLINE T fetch_and(T v, std::memory_order order = std::memory_order_seq_cst) noexcept {
    return base::fetch_and(v, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE T fetch_and(T v, std::memory_order order = std::memory_order_seq_cst) volatile noexcept {
    return base::fetch_and(v, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE T fetch_or(T v, std::memory_order order = std::memory_order_seq_cst) noexcept {
    return base::fetch_or(v, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE T fetch_or(T v, std::memory_order order = std::memory_order_seq_cst) volatile noexcept {
    return base::fetch_or(v, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE T fetch_xor(T v, std::memory_order order = std::memory_order_seq_cst) noexcept {
    return base::fetch_xor(v, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE T fetch_xor(T v, std::memory_order order = std::memory_order_seq_cst) volatile noexcept {
    return base::fetch_xor(v, preview::detail::to_boost_order(order));
  }
};

template<typename T>
using boost_atomic_adaptor_fetch_bitwise_op_base = std::conditional_t<
    std::is_integral<T>::value && !std::is_same<T, std::remove_cv_t<bool>>::value,
    boost_atomic_adaptor_fetch_bitwise_op<T, boost::atomic<T>>,
    boost::atomic<T>
>;

template<typename T, typename Base>
struct boost_atomic_adaptor_fetch_add_sub : Base {
 private:
  using base = Base;

  using base::fetch_add;
  using base::fetch_sub;

 public:
  using base::base;

  BOOST_FORCEINLINE typename base::value_type fetch_add(typename base::difference_type v, std::memory_order order = std::memory_order_seq_cst) noexcept {
    return base::fetch_add(v, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE typename base::value_type fetch_add(typename base::difference_type v, std::memory_order order = std::memory_order_seq_cst) volatile noexcept {
    return base::fetch_add(v, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE typename base::value_type fetch_sub(typename base::difference_type v, std::memory_order order = std::memory_order_seq_cst) noexcept {
    return base::fetch_sub(v, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE typename base::value_type fetch_sub(typename base::difference_type v, std::memory_order order = std::memory_order_seq_cst) volatile noexcept {
    return base::fetch_sub(v, preview::detail::to_boost_order(order));
  }
};

template<typename T>
using boost_atomic_adaptor_fetch_add_sub_base = std::conditional_t<
    has_typename_difference_type<boost::atomic<T>>::value,
    boost_atomic_adaptor_fetch_add_sub<T, boost_atomic_adaptor_fetch_bitwise_op_base<T>>,
    boost_atomic_adaptor_fetch_bitwise_op_base<T>
>;

template<typename T>
struct boost_atomic_adaptor : boost_atomic_adaptor_fetch_add_sub_base<T> {
 private:
  using base = boost_atomic_adaptor_fetch_add_sub_base<T>;

  using base::store;
  using base::load;
  using base::exchange;
  using base::compare_exchange_weak;
  using base::compare_exchange_strong;

 public:
  using base::base;

  BOOST_FORCEINLINE void store(T desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
    base::store(desired, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE void store(T desired, std::memory_order order = std::memory_order_seq_cst) volatile noexcept {
    base::store(desired, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE T load(std::memory_order order = std::memory_order_seq_cst) const noexcept {
    return base::load(preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE T load(std::memory_order order = std::memory_order_seq_cst) const volatile noexcept {
    return base::load(preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE T exchange(T desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
    return base::exchange(desired, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE T exchange(T desired, std::memory_order order = std::memory_order_seq_cst) volatile noexcept {
    return base::exchange(desired, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE bool compare_exchange_weak(T& expected, T desired, std::memory_order success, std::memory_order failure) noexcept {
    return base::compare_exchange_weak(
        expected,
        desired,
        preview::detail::to_boost_order(success),
        preview::detail::to_boost_order(failure)
    );
  }

  BOOST_FORCEINLINE bool compare_exchange_weak(T& expected, T desired, std::memory_order success, std::memory_order failure) volatile noexcept {
    return base::compare_exchange_weak(
        expected,
        desired,
        preview::detail::to_boost_order(success),
        preview::detail::to_boost_order(failure)
    );
  }

  BOOST_FORCEINLINE bool compare_exchange_weak(T& expected, T desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
    return base::compare_exchange_weak(expected, desired, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE bool compare_exchange_weak(T& expected, T desired, std::memory_order order = std::memory_order_seq_cst) volatile noexcept {
    return base::compare_exchange_weak(expected, desired, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE bool compare_exchange_strong(T& expected, T desired, std::memory_order success, std::memory_order failure) noexcept {
    return base::compare_exchange_strong(
        expected,
        desired,
        preview::detail::to_boost_order(success),
        preview::detail::to_boost_order(failure)
    );
  }

  BOOST_FORCEINLINE bool compare_exchange_strong(T& expected, T desired, std::memory_order success, std::memory_order failure) volatile noexcept {
    return base::compare_exchange_strong(
        expected,
        desired,
        preview::detail::to_boost_order(success),
        preview::detail::to_boost_order(failure)
    );
  }

  BOOST_FORCEINLINE bool compare_exchange_strong(T& expected, T desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
    return base::compare_exchange_strong(expected, desired, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE bool compare_exchange_strong(T& expected, T desired, std::memory_order order = std::memory_order_seq_cst) volatile noexcept {
    return base::compare_exchange_strong(expected, desired, preview::detail::to_boost_order(order));
  }
};

struct boost_atomic_flag_adaptor : private boost::atomic_flag {
 private:
  using base = boost::atomic_flag;
  // using base::clear;
  // using base::test_and_set;
  // using base::test;
  // using base::wait;
  // using base::notify_one;
  // using base::notify_all;

 public:
  constexpr boost_atomic_flag_adaptor() noexcept
      : base() {}

  boost_atomic_flag_adaptor(const boost_atomic_flag_adaptor&) = delete;
  boost_atomic_flag_adaptor& operator=( const boost_atomic_flag_adaptor& ) = delete;
  boost_atomic_flag_adaptor& operator=( const boost_atomic_flag_adaptor& ) volatile = delete;

  BOOST_FORCEINLINE void clear(std::memory_order order = std::memory_order_seq_cst) volatile noexcept {
    base::clear(preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE void clear(std::memory_order order = std::memory_order_seq_cst) noexcept {
    base::clear(preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE bool test_and_set(std::memory_order order = std::memory_order_seq_cst) volatile noexcept {
    return base::test_and_set(preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE bool test_and_set(std::memory_order order = std::memory_order_seq_cst) noexcept {
    return base::test_and_set(preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE bool test(std::memory_order order = std::memory_order_seq_cst) const volatile noexcept {
    return base::test(preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE bool test(std::memory_order order = std::memory_order_seq_cst) const noexcept {
    return base::test(preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE void wait(bool old, std::memory_order order = std::memory_order_seq_cst) const volatile noexcept {
    base::wait(old, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE void wait(bool old, std::memory_order order = std::memory_order_seq_cst) const noexcept {
    base::wait(old, preview::detail::to_boost_order(order));
  }

  BOOST_FORCEINLINE void notify_one() noexcept {
    base::notify_one();
  }

  BOOST_FORCEINLINE void notify_one() volatile noexcept {
    base::notify_one();
  }

  BOOST_FORCEINLINE void notify_all() noexcept {
    base::notify_one();
  }

  BOOST_FORCEINLINE void notify_all() volatile noexcept {
    base::notify_one();
  }
};

} // namespace detail
} // namespace preview

#endif // PREVIEW_ATOMIC_DETAIL_BOOST_ATOMIC_ADAPTOR_H_
