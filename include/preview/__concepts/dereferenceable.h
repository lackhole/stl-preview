//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_CONCEPTS_DEREFERENCEABLE_H_
#define PREVIEW_CONCEPTS_DEREFERENCEABLE_H_

#include <type_traits>

#include "preview/__type_traits/is_referenceable.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename T, typename = void>
struct dereferenceable_impl_3 : std::false_type {};

template<typename T>
struct dereferenceable_impl_3<T, void_t<decltype(*std::declval<T&>())>>
    : is_referencable<decltype(*std::declval<T&>())> {};

// Dereferencing a void* is illegal, but some compilers compiles these expression in unevaluated context.
// Omitting dereferenceable_impl_2 does not change the behavior of dereferenceable, but it is to remove warning
template<typename T, bool = std::is_void<std::remove_pointer_t<std::remove_reference_t<T>>>::value /* false */>
struct dereferenceable_impl_2 : dereferenceable_impl_3<T> {};

template<typename T>
struct dereferenceable_impl_2<T, true> : std::false_type {};

template<typename T, bool = is_referencable<T>::value /* true */>
struct dereferenceable_impl_1 : dereferenceable_impl_2<T> {};

template<typename T>
struct dereferenceable_impl_1<T, false> : std::false_type {};

} // namespace detail

template<typename T>
struct dereferenceable : detail::dereferenceable_impl_1<T> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_DEREFERENCEABLE_H_
