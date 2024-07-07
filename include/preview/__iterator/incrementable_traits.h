//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_ITERATOR_INCREMENTABLE_TRAITS_H_
#define PREVIEW_ITERATOR_INCREMENTABLE_TRAITS_H_

#include <cstddef>
#include <iterator>
#include <type_traits>

#include "preview/__concepts/subtractable.h"
#include "preview/__iterator/detail/have_cxx20_iterator.h"
#include "preview/__type_traits/has_typename_difference_type.h"
#include "preview/__type_traits/no_traits.h"

namespace preview {
namespace detail {

template<typename T, bool = subtractable<T>::value>
struct subtract_check : std::false_type {};

template<typename T>
struct subtract_check<T, true> : std::is_integral<decltype(std::declval<const T&>() - std::declval<const T&>())> {};


template<typename T>
struct incrementable_traits_subtractable {
  using difference_type = std::make_signed_t<decltype(std::declval<T>() - std::declval<T>())>;
};

template<typename T>
struct incrementable_traits_no_difference_type
    : std::conditional_t<
        subtract_check<T>::value,
        incrementable_traits_subtractable<T>,
        no_traits
      > {};

template<typename T>
struct incrementable_traits_yes_difference_type {
  using difference_type = typename T::difference_type;
};

template<typename T>
struct incrementable_traits_impl
    : std::conditional_t<
        has_typename_difference_type<T>::value,
        incrementable_traits_yes_difference_type<T>,
        incrementable_traits_no_difference_type<T>
      > {};

template<typename T>
struct incrementable_traits_object_pointer {
  using difference_type = std::ptrdiff_t;
};

template<typename T>
struct incrementable_traits_impl<T*>
    : std::conditional_t<
        std::is_object<T>::value,
        incrementable_traits_object_pointer<T>,
        no_traits
      > {};

#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
template<typename Container>
struct incrementable_traits_impl<std::back_insert_iterator<Container>> {
  using difference_type = std::ptrdiff_t;
};

template<typename Container>
struct incrementable_traits_impl<std::front_insert_iterator<Container>> {
  using difference_type = std::ptrdiff_t;
};

template<typename T>
struct incrementable_traits_impl<std::insert_iterator<T>> {
  using difference_type = std::ptrdiff_t;
};

template<typename T, typename CharT, typename Traits>
struct incrementable_traits_impl<std::ostream_iterator<T, CharT, Traits>> {
  using difference_type = std::ptrdiff_t;
};

template<typename T, typename Traits>
struct incrementable_traits_impl<std::ostreambuf_iterator<T, Traits>> {
  using difference_type = std::ptrdiff_t;
};
#endif

} // namespace detail

template<typename I>
struct incrementable_traits;

template<typename I>
struct incrementable_traits : detail::incrementable_traits_impl<I> {};

template<typename T>
struct incrementable_traits<const T> : incrementable_traits<T> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_INCREMENTABLE_TRAITS_H_
