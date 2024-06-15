//
// Created by yonggyulee on 2024/01/10.
//

#ifndef PREVIEW_ITERATOR_INDIRECTLY_MOVABLE_H_
#define PREVIEW_ITERATOR_INDIRECTLY_MOVABLE_H_

#include <type_traits>

#include "preview/__iterator/indirectly_readable.h"
#include "preview/__iterator/indirectly_writable.h"
#include "preview/__iterator/iter_rvalue_reference_t.h"

namespace preview {
namespace detail {

template<typename In, typename Out, bool = indirectly_readable<In>::value /* true */>
struct indirectly_movable_impl
    : indirectly_writable<Out, iter_rvalue_reference_t<In>> {};

template<typename In, typename Out>
struct indirectly_movable_impl<In, Out, false> : std::false_type {};

} // namespace detail

template<typename In, typename Out>
struct indirectly_movable : detail::indirectly_movable_impl<In, Out> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_INDIRECTLY_MOVABLE_H_
