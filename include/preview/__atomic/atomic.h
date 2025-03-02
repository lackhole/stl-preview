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

} // namespace preview


#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif // PREVIEW_ATOMIC_ATOMIC_H_
