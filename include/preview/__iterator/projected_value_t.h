//
// Created by cosge on 2024-07-09.
//

#ifndef PREVIEW_ITERATOR_PROJECTED_VALUE_T_H_
#define PREVIEW_ITERATOR_PROJECTED_VALUE_T_H_

#include "preview/__iterator/indirectly_readable.h"
#include "preview/__iterator/indirectly_regular_unary_invocable.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace detail {

template<typename I, typename Proj,
    bool = /* false */ conjunction<
        indirectly_readable<I>,
        indirectly_regular_unary_invocable<Proj, I>
        >::value
    >
struct projected_value_impl {};

template<typename I, typename Proj>
struct projected_value_impl<I, Proj, true> {
  using type = remove_cvref_t<invoke_result_t<Proj&, iter_value_t<I>&>>;
};

} // namespace detail

template<typename I, typename Proj>
struct projected_value : detail::projected_value_impl<I, Proj> {};

template<typename I, typename Proj>
using projected_value_t = typename projected_value<I, Proj>::type;

} // namespace preview

#endif // PREVIEW_ITERATOR_PROJECTED_VALUE_T_H_
