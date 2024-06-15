//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_CONCEPTS_MOVABLE_H_
#define PREVIEW_CONCEPTS_MOVABLE_H_

#include <type_traits>

#include "preview/__concepts/assignable_from.h"
#include "preview/__concepts/move_constructible.h"
#include "preview/__concepts/swappable.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_swappable.h"
#include "preview/__type_traits/is_referenceable.h"

namespace preview {
namespace detail {

template<typename T, bool = is_referencable<T>::value /* true */>
struct movable_impl
    : conjunction<
        std::is_object<T>,
        move_constructible<T>,
        assignable_from<T&, T>,
        swappable<T>
      > {};

template<typename T>
struct movable_impl<T, false> : std::false_type {};

} // namespace detail

template<typename T>
struct movable : detail::movable_impl<T> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_MOVABLE_H_
