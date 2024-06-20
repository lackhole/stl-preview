//
// Created by yonggyulee on 2023/12/21.
//

#ifndef PREVIEW_CONCEPTS_SUBTRACTABLE_H_
#define PREVIEW_CONCEPTS_SUBTRACTABLE_H_

#include <type_traits>

#include "preview/__concepts/requires_expression.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_referenceable.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename T, typename U, typename = void>
struct is_explicitly_subtractable : std::false_type {};

template<typename T, typename U>
struct is_explicitly_subtractable<T, U, void_t<decltype(std::declval<T>() - std::declval<U>())>> : std::true_type {};

template<typename T, bool = is_referencable<T>::value>
struct is_subtractable_impl : std::false_type {};

template<typename T>
struct is_subtractable_impl<T, true> : requires_expression<is_explicitly_subtractable, const T&, const T&> {};

} // namespace detail

template<typename T>
struct subtractable : detail::is_subtractable_impl<T> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_SUBTRACTABLE_H_
