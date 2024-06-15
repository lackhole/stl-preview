//
// Created by yonggyulee on 2024/01/10.
//

#ifndef PREVIEW_ITERATOR_INDIRECTLY_MOVABLE_STORABLE_H_
#define PREVIEW_ITERATOR_INDIRECTLY_MOVABLE_STORABLE_H_

#include <type_traits>

#include "preview/__concepts/assignable_from.h"
#include "preview/__concepts/constructible_from.h"
#include "preview/__concepts/movable.h"
#include "preview/__iterator/indirectly_movable.h"
#include "preview/__iterator/indirectly_writable.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__iterator/iter_rvalue_reference_t.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace detail {

template<typename In, typename Out, bool = indirectly_movable<In, Out>::value /* true */>
struct indirectly_movable_storable_impl
    : conjunction<
          indirectly_writable<Out, iter_value_t<In>>,
          movable<iter_value_t<In>>,
          constructible_from<iter_value_t<In>, iter_rvalue_reference_t<In>>,
          assignable_from<iter_value_t<In>&, iter_rvalue_reference_t<In>>
      >{};

template<typename In, typename Out>
struct indirectly_movable_storable_impl<In, Out, false> : std::false_type {};

} // namespace detail

template<typename In, typename Out>
struct indirectly_movable_storable : detail::indirectly_movable_storable_impl<In, Out> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_INDIRECTLY_MOVABLE_STORABLE_H_
