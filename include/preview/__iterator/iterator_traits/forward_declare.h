//
// Created by yonggyulee on 2023/12/25.
//

#ifndef PREVIEW_ITERATOR_ITERATOR_TRAITS_FORWARD_DECLARE_H_
#define PREVIEW_ITERATOR_ITERATOR_TRAITS_FORWARD_DECLARE_H_

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


// preview::iterator_traits

template<typename T> struct is_specialized_iterator_traits< iterator_traits<T> > : std::false_type {};
template<typename T> struct is_specialized_iterator_traits< iterator_traits<T*> > : std::is_object<T> {};

template<typename T>
using is_primary_iterator_traits = negation<is_specialized_iterator_traits<T>>;

} // namespace detail

} // namespace preview

#endif // PREVIEW_ITERATOR_ITERATOR_TRAITS_FORWARD_DECLARE_H_
