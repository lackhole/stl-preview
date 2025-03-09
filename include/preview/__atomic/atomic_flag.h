//
// Created by yonggyulee on 3/9/25.
//

#ifndef PREVIEW_ATOMIC_ATOMIC_FLAG_H_
#define PREVIEW_ATOMIC_ATOMIC_FLAG_H_

#include "preview/__atomic/detail/cxx20_atomic_base.h"

namespace preview {

struct atomic_flag : public detail::cxx20_atomic_flag_base {
  using detail::cxx20_atomic_flag_base::cxx20_atomic_flag_base;

  atomic_flag(const atomic_flag&) = delete;
  atomic_flag& operator=(const atomic_flag&) = delete;
  atomic_flag& operator=(const atomic_flag&) volatile = delete;

  using detail::cxx20_atomic_flag_base::clear;
  using detail::cxx20_atomic_flag_base::test_and_set;
  using detail::cxx20_atomic_flag_base::test;
  using detail::cxx20_atomic_flag_base::wait;
  using detail::cxx20_atomic_flag_base::notify_one;
  using detail::cxx20_atomic_flag_base::notify_all;
};

inline bool atomic_flag_test(const volatile atomic_flag* object) noexcept { return object->test(); }
inline bool atomic_flag_test(const          atomic_flag* object) noexcept { return object->test(); }
inline bool atomic_flag_test_explicit(const volatile atomic_flag* object, std::memory_order order) noexcept { return object->test(order); }
inline bool atomic_flag_test_explicit(const          atomic_flag* object, std::memory_order order) noexcept { return object->test(order); }

inline bool atomic_flag_test_and_set(volatile atomic_flag* obj) noexcept { return obj->test_and_set(); }
inline bool atomic_flag_test_and_set(         atomic_flag* obj) noexcept { return obj->test_and_set(); }
inline bool atomic_flag_test_and_set_explicit(volatile atomic_flag* obj, std::memory_order order) noexcept { return obj->test_and_set(order); }
inline bool atomic_flag_test_and_set_explicit(         atomic_flag* obj, std::memory_order order) noexcept { return obj->test_and_set(order); }

inline void atomic_flag_clear(volatile atomic_flag* obj) noexcept { obj->clear(); }
inline void atomic_flag_clear(         atomic_flag* obj) noexcept { obj->clear(); }
inline void atomic_flag_clear_explicit(volatile atomic_flag* obj, std::memory_order order) noexcept { obj->clear(order); }
inline void atomic_flag_clear_explicit(         atomic_flag* obj, std::memory_order order) noexcept { obj->clear(order); }

inline void atomic_flag_wait(volatile atomic_flag* obj, bool old) noexcept { obj->wait(old); }
inline void atomic_flag_wait(         atomic_flag* obj, bool old) noexcept { obj->wait(old); }
inline void atomic_flag_wait_explicit(volatile atomic_flag* obj, bool old, std::memory_order order) noexcept { obj->wait(old, order); }
inline void atomic_flag_wait_explicit(         atomic_flag* obj, bool old, std::memory_order order) noexcept { obj->wait(old, order); }

inline void atomic_flag_notify_one(atomic_flag* object) noexcept { return object->notify_one(); }
inline void atomic_flag_notify_one(volatile atomic_flag* object) noexcept { return object->notify_one(); }

inline void atomic_flag_notify_all(atomic_flag* object) noexcept { return object->notify_all(); }
inline void atomic_flag_notify_all(volatile atomic_flag* object) noexcept { return object->notify_all(); }

} // namespace preview

#endif // PREVIEW_ATOMIC_ATOMIC_FLAG_H_
