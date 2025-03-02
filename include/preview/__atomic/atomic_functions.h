//
// Created by YongGyuLee on 2025-03-02.
//

#ifndef PREVIEW_ATOMIC_ATOMIC_FUNCTIONS_H_
#define PREVIEW_ATOMIC_ATOMIC_FUNCTIONS_H_

#include <atomic>

#include "preview/__atomic/atomic.h"

namespace preview {

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

#endif // PREVIEW_ATOMIC_ATOMIC_FUNCTIONS_H_
