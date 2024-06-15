//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_CONCEPTS_WEAKLY_EQUALITY_COMPARABLE_WITH_H_
#define PREVIEW_CONCEPTS_WEAKLY_EQUALITY_COMPARABLE_WITH_H_

#include <type_traits>

#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_referenceable.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace detail {

template<
    typename T,
    typename U,
    bool = conjunction<
             is_referencable<std::remove_reference_t<T>>,
             is_referencable<std::remove_reference_t<U>>
           >::value
>
struct weakly_equality_comparable_with_impl : std::false_type {};

template<typename T, typename U>
struct weakly_equality_comparable_with_impl<T, U, true>
    : conjunction<
        rel_ops::is_equality_comparable<const std::remove_reference_t<T>&, const std::remove_reference_t<U>& >,
        rel_ops::is_equality_comparable<const std::remove_reference_t<U>&, const std::remove_reference_t<T>& >,
        rel_ops::is_non_equality_comparable<const std::remove_reference_t<T>&, const std::remove_reference_t<U>& >,
        rel_ops::is_non_equality_comparable<const std::remove_reference_t<U>&, const std::remove_reference_t<T>& >
      > {};

} // namespace detail

template<typename T, typename U>
struct weakly_equality_comparable_with : detail::weakly_equality_comparable_with_impl<T, U> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_WEAKLY_EQUALITY_COMPARABLE_WITH_H_
