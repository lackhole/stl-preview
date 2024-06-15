//
// Created by yonggyulee on 2023/12/29.
//

#ifndef PREVIEW_ITERATOR_INDIRECTLY_COMPARABLE_H_
#define PREVIEW_ITERATOR_INDIRECTLY_COMPARABLE_H_

#include <type_traits>

#include "preview/__functional/identity.h"
#include "preview/__iterator/indirect_binary_predicate.h"
#include "preview/__iterator/projected.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace detail {

template<
    typename I1,
    typename I2,
    typename Comp,
    typename Proj1,
    typename Proj2,
    bool = conjunction<projectable<I1, Proj1>, projectable<I2, Proj2>>::value /* true */
>
struct indirectly_comparable_impl
    : indirect_binary_predicate<Comp, projected<I1, Proj1>, projected<I2, Proj2>> {};

template<typename I1, typename I2, typename Comp, typename Proj1, typename Proj2>
struct indirectly_comparable_impl<I1, I2, Comp, Proj1, Proj2, false> : std::false_type {};

} // namespace detail

template<typename I1, typename I2, typename Comp,
         typename Proj1 = identity, typename Proj2 = identity>
struct indirectly_comparable : detail::indirectly_comparable_impl<I1, I2, Comp, Proj1, Proj2> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_INDIRECTLY_COMPARABLE_H_
