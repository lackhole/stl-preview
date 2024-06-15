//
// Created by yonggyulee on 1/26/24.
//

#ifndef PREVIEW_ITERATOR_INDIRECT_EQUIVALENCE_RELATION_H_
#define PREVIEW_ITERATOR_INDIRECT_EQUIVALENCE_RELATION_H_

#include <type_traits>

#include "preview/__concepts/copy_constructible.h"
#include "preview/__concepts/equivalence_relation.h"
#include "preview/__iterator/indirectly_readable.h"
#include "preview/__iterator/iter_common_reference_t.h"
#include "preview/__iterator/iter_reference_t.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace detail {

template<typename F, typename I1, typename I2>
struct indirect_equivalence_relation_impl_2
    : conjunction<
          equivalence_relation<F&, iter_value_t<I1>&, iter_value_t<I2>&>,
          equivalence_relation<F&, iter_value_t<I1>&, iter_reference_t<I2>>,
          equivalence_relation<F&, iter_reference_t<I1>, iter_value_t<I2>&>,
          equivalence_relation<F&, iter_reference_t<I1>, iter_reference_t<I2>>,
          equivalence_relation<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>
      > {};

template<typename F, typename I1, typename I2, bool = conjunction<
    indirectly_readable<I1>,
    indirectly_readable<I2>,
    copy_constructible<F>
>::value /* true */>
struct indirect_equivalence_relation_impl_1 : indirect_equivalence_relation_impl_2<F, I1, I2> {};
template<typename F, typename I1, typename I2>
struct indirect_equivalence_relation_impl_1<F, I1, I2, false> : std::false_type {};

} // namespace detail

template<typename F, typename I1, typename I2 = I1>
struct indirect_equivalence_relation : detail::indirect_equivalence_relation_impl_1<F, I1, I2> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_INDIRECT_EQUIVALENCE_RELATION_H_
