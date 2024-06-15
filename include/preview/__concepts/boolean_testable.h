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

template<typename B>
struct boolean_testable_impl : convertible_to<B, bool> {};

template<typename T, typename = void>
struct is_explicitly_negatable : std::false_type {};
template<typename T>
struct is_explicitly_negatable<T, void_t<decltype(!std::declval<T>())> > : std::true_type {};

template<
    typename B,
    bool =
        conjunction<
            is_explicitly_negatable<std::add_lvalue_reference_t<std::remove_reference_t<B>>>,
            is_explicitly_negatable<std::add_rvalue_reference_t<std::remove_reference_t<B>>>
        >::value
>
struct boolean_testable_stage_2 : std::false_type {};

template<typename B>
struct boolean_testable_stage_2<B, true>
    : conjunction<
        boolean_testable_impl<decltype(!std::declval< std::add_lvalue_reference_t<std::remove_reference_t<B>> >())>,
        boolean_testable_impl<decltype(!std::declval< std::add_rvalue_reference_t<std::remove_reference_t<B>> >())>
      > {};

template<
    typename B,
    bool = conjunction<boolean_testable_impl<B>, is_referencable<B>>::value
>
struct boolean_testable_stage_1 : std::false_type {};

template<typename B>
struct boolean_testable_stage_1<B, true> : boolean_testable_stage_2<B> {};

} // namespace detail

template<typename B>
struct boolean_testable : detail::boolean_testable_stage_1<B> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_BOOLEAN_TESTABLE_H_
