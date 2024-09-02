//
// Created by yonggyulee on 2024. 9. 2.
//

#ifndef PREVIEW_ATOMIC_DETAIL_CXX20_ATOMIC_H_
#define PREVIEW_ATOMIC_DETAIL_CXX20_ATOMIC_H_

#include <atomic>

#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_difference_type.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename T, typename = void>
struct has_member_function_wait : std::false_type {};
template<typename T>
struct has_member_function_wait<T, void_t<decltype(std::declval<T>().wait(std::declval<std::memory_order>()))>> : std::true_type {};

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

} // namespace detail
} // namespace preview

#endif // PREVIEW_ATOMIC_DETAIL_CXX20_ATOMIC_H_
