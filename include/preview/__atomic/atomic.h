//
// Created by yonggyulee on 2024. 7. 18.
//

#ifndef PREVIEW_ATOMIC_ATOMIC_H_
#define PREVIEW_ATOMIC_ATOMIC_H_

#include <memory>
#include <type_traits>
#include <utility>

#include "preview/__atomic/detail/atomic_floating_point.h"
#include "preview/__atomic/detail/atomic_generic.h"
#include "preview/__atomic/detail/atomic_integral.h"
#include "preview/__atomic/detail/atomic_pointer.h"
#include "preview/__atomic/detail/atomic_shared_ptr.h"
#include "preview/__atomic/detail/atomic_weak_ptr.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_complete.h"
#include "preview/__type_traits/negation.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4521) // multiple copy constructors specified
#pragma warning(disable:4522) // multiple assignment operators specified
#endif

namespace preview {
namespace detail {

template<typename T>
using atomic_base_t = conditional_t<
    conjunction<std::is_integral<T>, negation<std::is_same<bool, std::remove_cv_t<T>>>>, atomic_integral<T>,
    std::is_floating_point<T>, atomic_floating_point<T>,
    std::is_pointer<T>,        atomic_pointer<T>,
    atomic_generic<T>
>;

template<typename T>
using atomic_shared_ptr_base_t =
#if defined(__cpp_lib_atomic_shared_ptr) && __cpp_lib_atomic_shared_ptr >= 201711L
    std::atomic<std::shared_ptr<T>>;
#elif PREVIEW_CONFORM_CXX20_STANDARD
    conditional_t<
        is_complete<std::atomic<std::shared_ptr<T>>>, std::atomic<std::shared_ptr<T>>, atomic_shared_ptr<T>
    >;
#else
    atomic_shared_ptr<T>;
#endif

template<typename T>
using atomic_weak_ptr_base_t =
#if defined(__cpp_lib_atomic_shared_ptr) && __cpp_lib_atomic_shared_ptr >= 201711L
    std::atomic<std::weak_ptr<T>>;
#elif PREVIEW_CXX_VERSION >= 20
    std::atomic<std::weak_ptr<T>>;
#else
    atomic_weak_ptr<T>;
#endif

} // namespace detail

template<typename T>
struct atomic : detail::atomic_base_t<T> {
 private:
  using base = detail::atomic_base_t<T>;

 public:
  // static_assert(!base::is_always_lock_free && std::is_volatile<T>::value, "Ill-formed");

  template<typename Dummy = void, std::enable_if_t<std::is_void<Dummy>::value &&
      std::is_default_constructible<T>::value, int> = 0>
  constexpr atomic() noexcept(std::is_nothrow_default_constructible<T>::value)
      : base(T()) {}

  constexpr atomic(T desired)
      : base(desired) {}

  constexpr atomic(const atomic&) = delete;

  T operator=(T desired) noexcept {
      base::store(desired);
      return desired;
  }

  T operator=(T desired) volatile noexcept {
    base::store(desired);
    return desired;
  }

  atomic& operator=(const atomic& other) = delete;
  atomic& operator=(const atomic& other) volatile = delete;
};

template<typename T>
struct atomic<std::shared_ptr<T>> : detail::atomic_shared_ptr_base_t<T> {
 private:
  using base = detail::atomic_shared_ptr_base_t<T>;

 public:
  using value_type = std::shared_ptr<T>;

  constexpr atomic() noexcept = default;

  constexpr atomic(std::nullptr_t) noexcept
      : atomic() {}

  atomic(std::shared_ptr<T> desired) noexcept
    : base{std::move(desired)} {}

  atomic(atomic const&) = delete;
  atomic(atomic&&) = delete;

  void operator=(std::nullptr_t) noexcept {
    store(nullptr);
  }

  void operator=(std::shared_ptr<T> desired) noexcept {
    store(std::move(desired));
  }

  void operator=(atomic const&) = delete;
  void operator=(atomic&&) = delete;

  using base::is_lock_free;
  using base::store;
  using base::load;
  using base::operator std::shared_ptr<T>;
  using base::exchange;
  using base::compare_exchange_strong;
  using base::compare_exchange_weak;
};

template<typename T>
struct atomic<std::weak_ptr<T>> : detail::atomic_weak_ptr_base_t<T> {
 private:
  using base = detail::atomic_weak_ptr_base_t<T>;

 public:
  using base::base;
  using base::operator=;
};


/// atomic_store
template<typename T>
void atomic_store(volatile atomic<T>* object, typename atomic<T>::value_type desired) noexcept { object->store(desired); }
template<typename T>
constexpr void atomic_store(atomic<T>* object, typename atomic<T>::value_type desired) noexcept { object->store(desired); }
template<typename T>
void atomic_store_explicit(volatile atomic<T>* object, typename atomic<T>::value_type desired, std::memory_order order) noexcept { object->store(desired, order); }
template<typename T>
constexpr void atomic_store_explicit(atomic<T>* object, typename atomic<T>::value_type desired, std::memory_order order) noexcept { object->store(desired, order); }

/// atomic_load
template<typename T>
T atomic_load(const volatile atomic<T>* object) noexcept { return object->load(); }
template<typename T>
constexpr T atomic_load(const atomic<T>* object) noexcept { return object->load(); }
template<typename T>
T atomic_load_explicit(const volatile atomic<T>* object, std::memory_order order) noexcept { return object->load(order); }
template<typename T>
constexpr T atomic_load_explicit(const atomic<T>* object, std::memory_order order) noexcept { return object->load(order); }

/// atomic_exchange
template<typename T>
T atomic_exchange(volatile atomic<T>* object, typename atomic<T>::value_type desired) noexcept { return object->exchange(desired); }
template<typename T>
constexpr T atomic_exchange(atomic<T>* object, typename atomic<T>::value_type desired) noexcept { return object->exchange(desired); }
template<typename T>
T atomic_exchange_explicit(volatile atomic<T>* object, typename atomic<T>::value_type desired, std::memory_order order) noexcept { return object->exchange(desired, order); }
template<typename T>
constexpr T atomic_exchange_explicit(atomic<T>* object, typename atomic<T>::value_type desired, std::memory_order order) noexcept { return object->exchange(desired, order); }

/// atomic_compare_exchange_weak
template<typename T>
bool atomic_compare_exchange_weak(volatile atomic<T>* object, typename atomic<T>::value_type& expected, typename atomic<T>::value_type desired) noexcept { return object->compare_exchange_weak(expected, desired); }
template<typename T>
constexpr bool atomic_compare_exchange_weak(atomic<T>* object, typename atomic<T>::value_type& expected, typename atomic<T>::value_type desired) noexcept { return object->compare_exchange_weak(expected, desired); }
template<typename T>
bool atomic_compare_exchange_weak_explicit(volatile atomic<T>* object, typename atomic<T>::value_type& expected, typename atomic<T>::value_type desired, std::memory_order success, std::memory_order failure) noexcept { return object->compare_exchange_weak(expected, desired, success, failure); }
template<typename T>
constexpr bool atomic_compare_exchange_weak_explicit(atomic<T>* object, typename atomic<T>::value_type& expected, typename atomic<T>::value_type desired, std::memory_order success, std::memory_order failure) noexcept { return object->compare_exchange_weak(expected, desired, success, failure); }

/// atomic_compare_exchange_strong
template<typename T>
bool atomic_compare_exchange_strong(volatile atomic<T>* object, typename atomic<T>::value_type& expected, typename atomic<T>::value_type desired) noexcept { return object->compare_exchange_strong(expected, desired); }
template<typename T>
constexpr bool atomic_compare_exchange_strong(atomic<T>* object, typename atomic<T>::value_type& expected, typename atomic<T>::value_type desired) noexcept { return object->compare_exchange_strong(expected, desired); }
template<typename T>
bool atomic_compare_exchange_strong_explicit(volatile atomic<T>* object, typename atomic<T>::value_type& expected, typename atomic<T>::value_type desired, std::memory_order success, std::memory_order failure) noexcept { return object->compare_exchange_strong(expected, desired, success, failure); }
template<typename T>
constexpr bool atomic_compare_exchange_strong_explicit(atomic<T>* object, typename atomic<T>::value_type& expected, typename atomic<T>::value_type desired, std::memory_order success, std::memory_order failure) noexcept { return object->compare_exchange_strong(expected, desired, success, failure); }

/// atomic_fetch_add
template<typename T>
T atomic_fetch_add(volatile atomic<T>* object, typename atomic<T>::difference_type value) noexcept { return object->fetch_add(value); }
template<typename T>
constexpr T atomic_fetch_add(atomic<T>* object, typename atomic<T>::difference_type value) noexcept { return object->fetch_add(value); }
template<typename T>
T atomic_fetch_add_explicit(volatile atomic<T>* object, typename atomic<T>::difference_type value, std::memory_order order) noexcept { return object->fetch_add(value, order); }
template<typename T>
constexpr T atomic_fetch_add_explicit(atomic<T>* object, typename atomic<T>::difference_type value, std::memory_order order) noexcept { return object->fetch_add(value, order); }

/// atomic_fetch_sub
template<typename T>
T atomic_fetch_sub(volatile atomic<T>* object, typename atomic<T>::difference_type value) noexcept { return object->fetch_sub(value); }
template<typename T>
constexpr T atomic_fetch_sub(atomic<T>* object, typename atomic<T>::difference_type value) noexcept { return object->fetch_sub(value); }
template<typename T>
T atomic_fetch_sub_explicit(volatile atomic<T>* object, typename atomic<T>::difference_type value, std::memory_order order) noexcept { return object->fetch_sub(value, order); }
template<typename T>
constexpr T atomic_fetch_sub_explicit(atomic<T>* object, typename atomic<T>::difference_type value, std::memory_order order) noexcept { return object->fetch_sub(value, order); }

/// atomic_fetch_and
template<typename T>
T atomic_fetch_and(volatile atomic<T>* object, typename atomic<T>::difference_type value) noexcept { return object->fetch_and(value); }
template<typename T>
constexpr T atomic_fetch_and(atomic<T>* object, typename atomic<T>::difference_type value) noexcept { return object->fetch_and(value); }
template<typename T>
T atomic_fetch_and_explicit(volatile atomic<T>* object, typename atomic<T>::difference_type value, std::memory_order order) noexcept { return object->fetch_and(value, order); }
template<typename T>
constexpr T atomic_fetch_and_explicit(atomic<T>* object, typename atomic<T>::difference_type value, std::memory_order order) noexcept { return object->fetch_and(value, order); }

/// atomic_fetch_or
template<typename T>
T atomic_fetch_or(volatile atomic<T>* object, typename atomic<T>::difference_type value) noexcept { return object->fetch_or(value); }
template<typename T>
constexpr T atomic_fetch_or(atomic<T>* object, typename atomic<T>::difference_type value) noexcept { return object->fetch_or(value); }
template<typename T>
T atomic_fetch_or_explicit(volatile atomic<T>* object, typename atomic<T>::difference_type value, std::memory_order order) noexcept { return object->fetch_or(value, order); }
template<typename T>
constexpr T atomic_fetch_or_explicit(atomic<T>* object, typename atomic<T>::difference_type value, std::memory_order order) noexcept { return object->fetch_or(value, order); }

/// atomic_fetch_xor
template<typename T>
T atomic_fetch_xor(volatile atomic<T>* object, typename atomic<T>::difference_type value) noexcept { return object->fetch_xor(value); }
template<typename T>
constexpr T atomic_fetch_xor(atomic<T>* object, typename atomic<T>::difference_type value) noexcept { return object->fetch_xor(value); }
template<typename T>
T atomic_fetch_xor_explicit(volatile atomic<T>* object, typename atomic<T>::difference_type value, std::memory_order order) noexcept { return object->fetch_xor(value, order); }
template<typename T>
constexpr T atomic_fetch_xor_explicit(atomic<T>* object, typename atomic<T>::difference_type value, std::memory_order order) noexcept { return object->fetch_xor(value, order); }

/// atomic_fetch_max
template<typename T>
T atomic_fetch_max(volatile atomic<T>* object, typename atomic<T>::value_type arg) noexcept { return object->fetch_max(arg); }
template<typename T>
constexpr T atomic_fetch_max(atomic<T>* object, typename atomic<T>::value_type arg) noexcept { return object->fetch_max(arg); }
template<typename T>
T atomic_fetch_max_explicit(volatile atomic<T>* object, typename atomic<T>::value_type arg, std::memory_order order) noexcept { return object->fetch_max(arg, order); }
template<typename T>
constexpr T atomic_fetch_max_explicit(atomic<T>* object, typename atomic<T>::value_type arg, std::memory_order order) noexcept { return object->fetch_max(arg, order); }

/// atomic_fetch_min
template<typename T>
T atomic_fetch_min(volatile atomic<T>* object, typename atomic<T>::value_type arg) noexcept { return object->fetch_min(arg); }
template<typename T>
constexpr T atomic_fetch_min(atomic<T>* object, typename atomic<T>::value_type arg) noexcept { return object->fetch_min(arg); }
template<typename T>
T atomic_fetch_min_explicit(volatile atomic<T>* object, typename atomic<T>::value_type arg, std::memory_order order) noexcept { return object->fetch_min(arg, order); }
template<typename T>
constexpr T atomic_fetch_min_explicit(atomic<T>* object, typename atomic<T>::value_type arg, std::memory_order order) noexcept { return object->fetch_min(arg, order); }

/// atomic_wait
template<typename T>
void atomic_wait(const volatile atomic<T>* object, typename atomic<T>::value_type old) { object->wait(old); }
template<typename T>
constexpr void atomic_wait(const atomic<T>* object, typename atomic<T>::value_type old) { object->wait(old); }
template<typename T>
void atomic_wait_explicit(const volatile atomic<T>* object, typename atomic<T>::value_type old, std::memory_order order) { object->wait(old, order); }
template<typename T>
constexpr void atomic_wait_explicit(const atomic<T>* object, typename atomic<T>::value_type old, std::memory_order order) { object->wait(old, order); }

/// atomic_notify_one
template<typename T>
void atomic_notify_one(volatile atomic<T>* object) { object->notify_one(); }
template<typename T>
constexpr void atomic_notify_one(atomic<T>* object) { object->notify_one(); }

/// atomic_notify_all
template<typename T>
void atomic_notify_all(volatile atomic<T>* object) { object->notify_all(); }
template<typename T>
constexpr void atomic_notify_all(atomic<T>* object) { object->notify_all(); }

} // namespace preview

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif // PREVIEW_ATOMIC_ATOMIC_H_
