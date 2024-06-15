//
// Created by yonggyulee on 2023/12/25.
//

#ifndef PREVIEW_ITERATOR_ITERATOR_TRAITS_FORWARD_DECLARE_H_
#define PREVIEW_ITERATOR_ITERATOR_TRAITS_FORWARD_DECLARE_H_

#if __cplusplus >= 202002L
#include <iterator>
#endif
#include <type_traits>

#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/void_t.h"

namespace preview {

template<typename Iter>
struct iterator_traits;

namespace detail {

template<typename T>
struct is_specialized_iterator_traits;

// std::iterator_traits

template<typename T> struct is_specialized_iterator_traits< std::iterator_traits<T> > : std::false_type {};
template<typename T> struct is_specialized_iterator_traits< std::iterator_traits<T*> > : std::is_object<T> {};
#if __cplusplus >= 202002L
template<std::input_iterator I>
struct is_specialized_iterator_traits<std::iterator_traits< std::counted_iterator<I> >> : is_specialized_iterator_traits<I> {};
template<typename I, typename S>
struct is_specialized_iterator_traits<std::iterator_traits< std::common_iterator<I, S> >> : std::true_type {};
#endif


// preview::cxx20_iterator_traits

template<typename T> struct is_specialized_iterator_traits< iterator_traits<T> > : std::false_type {};
template<typename T> struct is_specialized_iterator_traits< iterator_traits<T*> > : std::is_object<T> {};

#if __cplusplus >= 202002L
template<std::input_iterator I>
struct is_specialized_iterator_traits<iterator_traits<std::counted_iterator<I> >> : is_specialized_iterator_traits<I> {};
template<typename I, typename S>
struct is_specialized_iterator_traits<iterator_traits<std::common_iterator<I, S> >> : std::true_type {};
#endif

template<typename T>
using is_primary_iterator_traits = negation<is_specialized_iterator_traits<T>>;

} // namespace detail

} // namespace preview

#endif // PREVIEW_ITERATOR_ITERATOR_TRAITS_FORWARD_DECLARE_H_
