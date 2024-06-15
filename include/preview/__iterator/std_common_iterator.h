//
// Created by lux on 2024. 6. 15..
//

#ifndef PREVIEW_ITERATOR_STD_COMMON_ITERATOR_H_
#define PREVIEW_ITERATOR_STD_COMMON_ITERATOR_H_

#include <iterator>
#include <type_traits>

#include "preview/core.h"
#include "preview/__iterator/iterator_traits/forward_declare.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/input_iterator.h"

namespace preview {
namespace detail {

#if PREVIEW_CXX_VERSION >= 20
template<typename I>
concept input_iterator_concept = requires (I i) {
  preview::input_iterator<I>::value;
};

template<input_iterator_concept I>
struct is_specialized_iterator_traits<std::iterator_traits< std::counted_iterator<I> >> : is_specialized_iterator_traits<I> {};

template<input_iterator_concept I>
struct is_specialized_iterator_traits<iterator_traits<std::counted_iterator<I> >> : is_specialized_iterator_traits<I> {};
#endif

} // namespace detail
} // namespace preview

#endif // PREVIEW_ITERATOR_STD_COMMON_ITERATOR_H_
