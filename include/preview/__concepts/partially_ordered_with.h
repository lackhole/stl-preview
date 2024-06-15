//
// Created by yonggyulee on 2023/12/25.
//

#ifndef PREVIEW_CONCEPTS_PARTIALLY_ORDERED_WITH_H_
#define PREVIEW_CONCEPTS_PARTIALLY_ORDERED_WITH_H_

#include <type_traits>

#include "preview/__concepts/implicit_expression_check.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_referenceable.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace detail {

template<typename T, typename U, bool = conjunction<is_referencable<T>, is_referencable<U>>::value>
struct partially_ordered_with_impl : std::false_type {};
template<typename T, typename U>
struct partially_ordered_with_impl<T, U, true>
    : conjunction<
        implicit_expression_check<rel_ops::is_less_than_comparable,          const std::remove_reference_t<T>&, const std::remove_reference_t<U>& >,
        implicit_expression_check<rel_ops::is_less_equal_than_comparable,    const std::remove_reference_t<T>&, const std::remove_reference_t<U>& >,
        implicit_expression_check<rel_ops::is_greater_than_comparable,       const std::remove_reference_t<T>&, const std::remove_reference_t<U>& >,
        implicit_expression_check<rel_ops::is_greater_equal_than_comparable, const std::remove_reference_t<T>&, const std::remove_reference_t<U>& >,
        implicit_expression_check<rel_ops::is_less_than_comparable,          const std::remove_reference_t<U>&, const std::remove_reference_t<T>& >,
        implicit_expression_check<rel_ops::is_less_equal_than_comparable,    const std::remove_reference_t<U>&, const std::remove_reference_t<T>& >,
        implicit_expression_check<rel_ops::is_greater_than_comparable,       const std::remove_reference_t<U>&, const std::remove_reference_t<T>& >,
        implicit_expression_check<rel_ops::is_greater_equal_than_comparable, const std::remove_reference_t<U>&, const std::remove_reference_t<T>& >
      > {};

} // namespace detail

template<typename T, typename U>
struct partially_ordered_with : detail::partially_ordered_with_impl<T, U> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_PARTIALLY_ORDERED_WITH_H_
