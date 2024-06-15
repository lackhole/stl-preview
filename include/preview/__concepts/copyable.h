//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_CONCEPTS_COPYABLE_H_
#define PREVIEW_CONCEPTS_COPYABLE_H_

#include <type_traits>

#include "preview/__concepts/assignable_from.h"
#include "preview/__concepts/copy_constructible.h"
#include "preview/__concepts/movable.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_referenceable.h"

namespace preview {
namespace detail {

template<typename T, bool = is_referencable<T>::value>
struct copyable_impl
    : conjunction<
        copy_constructible<T>,
        movable<T>,
        assignable_from<T&, T&>,
        assignable_from<T&, const T&>,
        assignable_from<T&, const T>
      > {};

template<typename T>
struct copyable_impl<T, false> : std::false_type {};

} // namespace detail

template<typename T>
struct copyable : detail::copyable_impl<T> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_COPYABLE_H_
