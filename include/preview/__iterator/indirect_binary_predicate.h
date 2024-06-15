//
// Created by yonggyulee on 2023/12/29.
//

#ifndef PREVIEW_ITERATOR_INDIRECT_BINARY_PREDICATE_H_
#define PREVIEW_ITERATOR_INDIRECT_BINARY_PREDICATE_H_

#include <type_traits>

#include "preview/__concepts/copy_constructible.h"
#include "preview/__concepts/predicate.h"
#include "preview/__iterator/iter_common_reference_t.h"
#include "preview/__iterator/indirect_result_t.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace detail {

template<
    typename F,
    typename I1,
    typename I2,
    bool = conjunction<
               has_typename_type<iter_common_reference<I1>>,
               has_typename_type<iter_common_reference<I2>>
           >::value /* true */
>
struct indirect_binary_predicate_impl_2
    : conjunction<
          predicate<F&, iter_value_t<I1>&, iter_value_t<I2>&>,
          predicate<F&, iter_value_t<I1>&, iter_reference_t<I2>>,
          predicate<F&, iter_reference_t<I1>&, iter_value_t<I2>&>,
          predicate<F&, iter_reference_t<I1>&, iter_reference_t<I2>>,
          predicate<F&, iter_common_reference_t<I1>&, iter_common_reference_t<I2>>
      >{};

template<typename F, typename I1, typename I2>
struct indirect_binary_predicate_impl_2<F, I1, I2, false> : std::false_type {};


template<
    typename F,
    typename I1,
    typename I2,
    bool = conjunction<
               indirectly_readable<I1>,
               indirectly_readable<I2>,
               copy_constructible<F>
           >::value /* true */
>
struct indirect_binary_predicate_impl_1 : indirect_binary_predicate_impl_2<F, I1, I2> {};

template<typename F, typename I1, typename I2>
struct indirect_binary_predicate_impl_1<F, I1, I2, false> : std::false_type {};

} // namespace detail

template<typename F, typename I1, typename I2>
struct indirect_binary_predicate : detail::indirect_binary_predicate_impl_1<F, I1, I2> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_INDIRECT_BINARY_PREDICATE_H_
