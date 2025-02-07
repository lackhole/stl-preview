//
// Created by yonggyulee on 2024. 9. 2.
//

#ifndef PREVIEW_ATOMIC_DETAIL_ATOMIC_BASE_H_
#define PREVIEW_ATOMIC_DETAIL_ATOMIC_BASE_H_

#include <atomic>
#include <type_traits>

#include "preview/config.h"

#if PREVIEW_USE_BOOST_ATOMIC
#include "boost/atomic.hpp"
#endif

#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_difference_type.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename T, typename = void>
struct has_member_function_wait : std::false_type {};
template<typename T>
struct has_member_function_wait<T, void_t<decltype(std::declval<T>().wait(std::declval<T>(), std::declval<std::memory_order>()))>> : std::true_type {};

template<typename T, typename = void>
struct has_member_function_notify_one : std::false_type {};
template<typename T>
struct has_member_function_notify_one<T, void_t<decltype(std::declval<T>().notify_one())>> : std::true_type {};

template<typename T, typename = void>
struct has_member_function_notify_all : std::false_type {};
template<typename T>
struct has_member_function_notify_all<T, void_t<decltype(std::declval<T>().notify_all())>> : std::true_type {};

template<typename T>
struct has_wait_and_notify : conjunction<
    has_member_function_wait<T>,
    has_member_function_notify_one<T>,
    has_member_function_notify_all<T>
> {};

template<typename T>
struct is_cxx20_atomic : conjunction<
    has_wait_and_notify<T>,
    has_typename_difference_type<T>
> {};

template<typename T>
using cxx20_atomic_base =
#if PREVIEW_USE_BOOST_ATOMIC
    boost::atomic<T>;
#else
    std::atomic<T>;
    static_assert(is_cxx20_atomic<std::atomic<int>>::value,
        "std::atomic<T> does not conform C++20 standard. Set PREVIEW_USE_BOOST_ATOMIC to 1");
#endif



} // namespace detail
} // namespace preview

#endif // PREVIEW_ATOMIC_DETAIL_ATOMIC_BASE_H_
