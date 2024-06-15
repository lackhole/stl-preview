//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_CONCEPTS_COPY_CONSTRUCTIBLE_H_
#define PREVIEW_CONCEPTS_COPY_CONSTRUCTIBLE_H_

#include "preview/__concepts/move_constructible.h"
#include "preview/__concepts/constructible_from.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_referenceable.h"

namespace preview {
namespace detail {

template<typename T, bool = is_referencable<T>::value>
struct copy_constructible_impl
    : conjunction<
        move_constructible<T>,
        constructible_from<T, T&>, convertible_to<T&, T>,
        constructible_from<T, const T&>, convertible_to<const T&, T>,
        constructible_from<T, const T>, convertible_to<const T, T>
      > {};

template<typename T>
struct copy_constructible_impl<T, false> : std::false_type {};

} // namespace detail

template<typename T>
struct copy_constructible : detail::copy_constructible_impl<T> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_COPY_CONSTRUCTIBLE_H_
