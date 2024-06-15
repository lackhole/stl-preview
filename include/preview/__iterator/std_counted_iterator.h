//
// Created by YongGyu Lee on 2024. 6. 15..
//

#ifndef PREVIEW_ITERATOR_STD_COUNTED_ITERATOR_H_
#define PREVIEW_ITERATOR_STD_COUNTED_ITERATOR_H_

#include "preview/__iterator/detail/std_check.h"

#if PREVIEW_STD_HAVE_CXX20_ITERATOR

#include <iterator>

#include "preview/core.h"
#include "preview/__iterator/iterator_traits/forward_declare.h"

namespace preview {
namespace detail {

template<typename I, typename S>
struct is_specialized_iterator_traits<std::iterator_traits< std::common_iterator<I, S> >> : std::true_type {};

template<typename I, typename S>
struct is_specialized_iterator_traits<iterator_traits<std::common_iterator<I, S> >> : std::true_type {};

} // namespace detail
} // namespace preview

#endif

#endif // PREVIEW_ITERATOR_STD_COUNTED_ITERATOR_H_
