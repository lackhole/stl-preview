//
// Created by YongGyuLee on 2025-03-01.
//

#ifndef PREVIEW_ATOMIC_DETAIL_ATOMIC_SHARED_PTR_H_
#define PREVIEW_ATOMIC_DETAIL_ATOMIC_SHARED_PTR_H_

#include <cstddef>
#include <memory>
#include <utility>

#include "preview/__core/nodiscard.h"

namespace preview {
namespace detail {

// Note: This is pre-C++20 fallback of `preview::atomic<shared_ptr<T>>`.
template<typename T>
struct atomic_shared_ptr {
  using value_type = std::shared_ptr<T>;

  // MSVC, GCC, Clang all define this as false
  static constexpr bool is_always_lock_free = false;

  constexpr atomic_shared_ptr() noexcept = default;

  constexpr atomic_shared_ptr(std::nullptr_t) noexcept
      : atomic_shared_ptr() {}

  atomic_shared_ptr(std::shared_ptr<T> desired) noexcept
      : ptr_{std::move(desired)} {}

  atomic_shared_ptr(atomic_shared_ptr const&) = delete;
  atomic_shared_ptr(atomic_shared_ptr&&) = delete;

  void operator=(std::nullptr_t) noexcept {
    store(nullptr);
  }

  void operator=(std::shared_ptr<T> desired) noexcept {
    store(std::move(desired));
  }

  void operator=(atomic_shared_ptr const&) = delete;
  void operator=(atomic_shared_ptr&&) = delete;

  PREVIEW_NODISCARD bool is_lock_free() const noexcept {
    return std::atomic_is_lock_free(&ptr_);
  }

  void store(std::shared_ptr<T> desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
    std::atomic_store_explicit(&ptr_, std::move(desired), order);
  }

  std::shared_ptr<T> load(std::memory_order order = std::memory_order_seq_cst) const noexcept {
    return std::atomic_load_explicit(&ptr_, order);
  }

  operator std::shared_ptr<T>() const noexcept {
    return load();
  }

  std::shared_ptr<T> exchange(std::shared_ptr<T> desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
    return std::atomic_exchange_explicit(&ptr_, std::move(desired), order);
  }

  bool compare_exchange_strong(std::shared_ptr<T>& expected, std::shared_ptr<T> desired, std::memory_order success, std::memory_order failure) noexcept {
    return std::atomic_compare_exchange_strong_explicit(&ptr_, &expected, std::move(desired), success, failure);
  }

  bool compare_exchange_weak(std::shared_ptr<T>& expected, std::shared_ptr<T> desired, std::memory_order success, std::memory_order failure) noexcept {
    return std::atomic_compare_exchange_weak_explicit(&ptr_, &expected, std::move(desired), success, failure);
  }

  bool compare_exchange_strong(std::shared_ptr<T>& expected, std::shared_ptr<T> desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
    return std::atomic_compare_exchange_strong_explicit(&ptr_, &expected, std::move(desired), order, preview::detail::memory_order_load(order));
  }

  bool compare_exchange_weak(std::shared_ptr<T>& expected, std::shared_ptr<T> desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
    return std::atomic_compare_exchange_weak_explicit(&ptr_, &expected, std::move(desired), order, preview::detail::memory_order_load(order));
  }

 private:
  std::shared_ptr<T> ptr_;
};

} // namespace detail
} // namespace preview

#endif // PREVIEW_ATOMIC_DETAIL_ATOMIC_SHARED_PTR_H_
