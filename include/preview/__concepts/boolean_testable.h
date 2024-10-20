//
// Created by yonggyulee on 2023/12/25.
//

#ifndef PREVIEW_CONCEPTS_BOOLEAN_TESTABLE_H_
#define PREVIEW_CONCEPTS_BOOLEAN_TESTABLE_H_

#include <type_traits>

#include "preview/__concepts/convertible_to.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_referenceable.h"

namespace preview {
namespace detail {

template<typename T>
struct boolean_testable_impl : convertible_to<T, bool> {};

template<typename T, typename = void>
struct is_explicitly_negatable : std::false_type {};
template<typename T>
struct is_explicitly_negatable<T, void_t<decltype(!std::declval<T>())> > : std::true_type {};

template<typename T, bool = /* false */ is_explicitly_negatable<T>::value>
struct boolean_testable_stage_2 : std::false_type {};
template<typename T>
struct boolean_testable_stage_2<T, true> : boolean_testable_impl<decltype(!std::declval<T>())> {};

template<typename T, bool = /* false */ boolean_testable_impl<T>::value>
struct boolean_testable_stage_1 : std::false_type {};
template<typename T>
struct boolean_testable_stage_1<T, true> : boolean_testable_stage_2<T> {};

} // namespace detail

// TODO: concept
template<typename T>
struct boolean_testable : detail::boolean_testable_stage_1<T> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_BOOLEAN_TESTABLE_H_
