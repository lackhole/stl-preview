//
// Created by yonggyulee on 2023/12/31.
//

#ifndef PREVIEW_ITERATOR_INDIRECTLY_WRITABLE_H_
#define PREVIEW_ITERATOR_INDIRECTLY_WRITABLE_H_

#include <type_traits>
#include <utility>

#include "preview/__concepts/dereferenceable.h"
#include "preview/__iterator/iter_reference_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_referenceable.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<
    typename Out,
    typename T,
    bool = conjunction<
               dereferenceable<decltype(std::forward<Out>(std::declval<Out&&>()))>,
               is_referencable<iter_reference_t<Out>>
           >::value /* true */
>
struct indirectly_writable_impl_2
    : conjunction<
          std::is_assignable<decltype(*std::declval<Out&&>()), decltype(std::forward<T>(std::declval<T&&>()))>,
          std::is_assignable<decltype(*std::forward<Out>(std::declval<Out&&>())), decltype(std::forward<T>(std::declval<T&&>()))>,
          std::is_assignable<decltype(const_cast<const iter_reference_t<Out>&&>(*std::declval<Out>())), decltype(std::forward<T>(std::declval<T&&>()))>,
          std::is_assignable<decltype(const_cast<const iter_reference_t<Out>&&>(*std::forward<Out>(std::declval<Out>()))), decltype(std::forward<T>(std::declval<T&&>()))>
      >{};

template<typename Out, typename T>
struct indirectly_writable_impl_2<Out, T, false> : std::false_type {};

template<
    typename Out,
    typename T,
    bool = conjunction<
               dereferenceable<Out>,
               is_referencable<T>,
               has_typename_type<iter_reference<Out>>
           >::value /* true */
>
struct indirectly_writable_impl : indirectly_writable_impl_2<Out, T> {};
template<typename Out, typename T>
struct indirectly_writable_impl<Out, T, false> : std::false_type {};

} // namespace detail

template<typename Out, typename T>
struct indirectly_writable : detail::indirectly_writable_impl<Out, T> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_INDIRECTLY_WRITABLE_H_
