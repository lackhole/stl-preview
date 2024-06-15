//
// Created by yonggyulee on 2024/01/12.
//

#ifndef PREVIEW_ITERATOR_INDIRECT_UNARY_PREDICATE_H_
#define PREVIEW_ITERATOR_INDIRECT_UNARY_PREDICATE_H_

#include <type_traits>

#include "preview/__concepts/copy_constructible.h"
#include "preview/__concepts/predicate.h"
#include "preview/__iterator/indirectly_readable.h"
#include "preview/__iterator/iter_common_reference_t.h"
#include "preview/__iterator/iter_reference_t.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace detail {

template<typename F, typename I, bool = conjunction<indirectly_readable<I>, copy_constructible<F>>::value /* false */>
struct indirect_unary_predicate_impl : std::false_type {};

template<typename F, typename I>
struct indirect_unary_predicate_impl<F, I, true>
    : conjunction<
          predicate<F&, iter_value_t<I>>,
          predicate<F&, iter_reference_t<I>>,
          predicate<F&, iter_common_reference_t<I>>
      > {};

} // namespace detail

template<typename F, typename I>
struct indirect_unary_predicate : detail::indirect_unary_predicate_impl<F, I> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_INDIRECT_UNARY_PREDICATE_H_
