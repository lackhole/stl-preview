//
// Created by YongGyu Lee on 2024. 6. 15..
//

#ifndef PREVIEW_ITERATOR_STD_COMMON_ITERATOR_H_
#define PREVIEW_ITERATOR_STD_COMMON_ITERATOR_H_

#include "preview/__iterator/detail/have_cxx20_iterator.h"

#if PREVIEW_STD_HAVE_CXX20_ITERATOR

#include <iterator>
#include <type_traits>

#include "preview/__iterator/iterator_traits/forward_declare.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/input_iterator.h"

namespace preview {
namespace detail {

template<typename I>
concept input_iterator_concept = requires (I i) {
  preview::input_iterator<I>::value;
};

template<input_iterator_concept I>
struct is_specialized_iterator_traits<std::iterator_traits< std::counted_iterator<I> >> : is_specialized_iterator_traits<I> {};

template<input_iterator_concept I>
struct is_specialized_iterator_traits<iterator_traits<std::counted_iterator<I> >> : is_specialized_iterator_traits<I> {};

} // namespace detail
} // namespace preview

#endif

#endif // PREVIEW_ITERATOR_STD_COMMON_ITERATOR_H_
