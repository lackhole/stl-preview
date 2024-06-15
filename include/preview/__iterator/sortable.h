//
// Created by yonggyulee on 1/26/24.
//

#ifndef PREVIEW_ITERATOR_SORTABLE_H_
#define PREVIEW_ITERATOR_SORTABLE_H_

#include <type_traits>

#include "preview/__functional/identity.h"
#include "preview/__functional/less.h"
#include "preview/__iterator/indirect_strict_weak_order.h"
#include "preview/__iterator/permutable.h"
#include "preview/__iterator/projected.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace detail {

template<typename I, typename Comp, typename Proj,
    bool = conjunction<
        permutable<I>,
        projectable<I, Proj>
    >::value /* false */>
struct sortable_impl : std::false_type {};

template<typename I, typename Comp, typename Proj>
struct sortable_impl<I, Comp, Proj, true>
    : indirect_strict_weak_order<Comp, projected<I, Proj>> {};

} // namespace detail

template<typename I, typename Comp = ranges::less, typename Proj = identity>
struct sortable : detail::sortable_impl<I, Comp, Proj> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_SORTABLE_H_
