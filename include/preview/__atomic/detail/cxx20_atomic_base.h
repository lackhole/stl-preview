//
// Created by yonggyulee on 2024. 9. 2.
//

#ifndef PREVIEW_ATOMIC_DETAIL_ATOMIC_BASE_H_
#define PREVIEW_ATOMIC_DETAIL_ATOMIC_BASE_H_

#include <atomic>
#include <type_traits>

#include "preview/config.h"

#if PREVIEW_USE_BOOST_ATOMIC
#include "preview/__atomic/detail/boost_atomic_adaptor.h"
#endif

#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/has_typename_difference_type.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

constexpr std::memory_order memory_order_load(std::memory_order order) noexcept {
    return order == std::memory_order_release ? std::memory_order_relaxed :
           order == std::memory_order_acq_rel ? std::memory_order_acquire :
           order;
}

constexpr std::memory_order memory_order_drop_release(std::memory_order order) noexcept {
    return order == std::memory_order_release ? std::memory_order_relaxed :
           (order == std::memory_order_acq_rel || order == std::memory_order_seq_cst) ? std::memory_order_acquire :
           order;
}

template<typename T, typename = void>
struct has_member_function_wait : std::false_type {};
template<typename T>
struct has_member_function_wait<T, void_t<decltype(std::declval<T>().wait(std::declval<typename T::value_type>()))>> : std::true_type {};

template<typename T, typename = void>
struct has_member_function_notify_one : std::false_type {};
template<typename T>
struct has_member_function_notify_one<T, void_t<decltype(std::declval<T>().notify_one())>> : std::true_type {};

template<typename T, typename = void>
struct has_member_function_notify_all : std::false_type {};
template<typename T>
struct has_member_function_notify_all<T, void_t<decltype(std::declval<T>().notify_all())>> : std::true_type {};

template<typename T, typename = void>
struct has_member_function_fetch_max : std::false_type {};
template<typename T>
struct has_member_function_fetch_max<
    T,
    void_t<decltype(std::declval<T&>().fetch_max(std::declval<typename T::value_type>()))>
> : std::true_type {};

template<typename T, typename = void>
struct has_member_function_fetch_min : std::false_type {};
template<typename T>
struct has_member_function_fetch_min<
    T,
    void_t<decltype(std::declval<T&>().fetch_min(std::declval<typename T::value_type>()))>
> : std::true_type {};

template<typename T>
struct has_wait_and_notify : conjunction<
    has_member_function_wait<T>,
    has_member_function_notify_one<T>,
    has_member_function_notify_all<T>
> {};

template<typename T>
struct has_fetch_max_and_fetch_min : conjunction<
    has_member_function_fetch_max<T>,
    has_member_function_fetch_min<T>
> {};

template<typename T>
struct is_cxx20_atomic : has_wait_and_notify<T> {};

template<typename T>
using cxx20_atomic_base =
#if PREVIEW_USE_BOOST_ATOMIC
#  if PREVIEW_PREFER_STL_ATOMIC
    std::conditional_t<is_cxx20_atomic<std::atomic<T>>::value, std::atomic<T>,  boost_atomic_adaptor<T>>;
#  else
    boost_atomic_adaptor<T>;
#  endif
#else
    std::atomic<T>;
#endif

} // namespace detail
} // namespace preview

#endif // PREVIEW_ATOMIC_DETAIL_ATOMIC_BASE_H_
