//
// Created by yonggyulee on 2023/12/27.
//

#ifndef PREVIEW_CONCEPTS_PREDICATE_H_
#define PREVIEW_CONCEPTS_PREDICATE_H_

#include "preview/__concepts/boolean_testable.h"
#include "preview/__concepts/invocable.h"
#include "preview/__functional/invoke.h"

namespace preview {
namespace detail {

template<bool v /* false */, typename F, typename... Args>
struct predicate_impl : std::false_type {};

template<typename F, typename... Args>
struct predicate_impl<true, F, Args...> : boolean_testable<invoke_result_t<F, Args...>> {};

} // namespace detail

template<typename F, typename... Args>
struct predicate : detail::predicate_impl<regular_invocable<F, Args...>::value, F, Args...> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_PREDICATE_H_
