//
// Created by yonggyulee on 1/26/24.
//

#ifndef PREVIEW_ITERATOR_MERGEABLE_H_
#define PREVIEW_ITERATOR_MERGEABLE_H_

#include <type_traits>

#include "preview/__functional/identity.h"
#include "preview/__functional/less.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/indirect_strict_weak_order.h"
#include "preview/__iterator/indirectly_copyable.h"
#include "preview/__iterator/projected.h"
#include "preview/__iterator/weakly_incrementable.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace detail {

template<
    typename I1, typename I2, typename Out,
    typename Comp, typename Proj1, typename Proj2,
    bool = conjunction<
        input_iterator<I1>, input_iterator<I2>,
        projectable<I1, Proj1>, projectable<I2, Proj2>
    >::value /* false */
>
struct mergeable_impl_1 : std::false_type {};

template<typename I1, typename I2, typename Out, typename Comp, typename Proj1, typename Proj2>
struct mergeable_impl_1<I1, I2, Out, Comp, Proj1, Proj2, true>
    : conjunction<
          weakly_incrementable<Out>,
          indirectly_copyable<I1, Out>,
          indirectly_copyable<I2, Out>,
          indirect_strict_weak_order<Comp, projected<I1, Proj1>, projected<I2, Proj2>>
      > {};

} // namespace detail

template<typename I1, typename I2, typename Out,
    typename Comp = ranges::less, typename Proj1 = identity, typename Proj2 = identity>
struct mergeable : detail::mergeable_impl_1<I1, I2, Out, Comp, Proj1, Proj2> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_MERGEABLE_H_
