//
// Created by lux on 2024. 6. 15..
//

#ifndef PREVIEW_ITERATOR_STD_COUNTED_ITERATOR_H_
#define PREVIEW_ITERATOR_STD_COUNTED_ITERATOR_H_

#include <iterator>

#include "preview/core.h"
#include "preview/__iterator/iterator_traits/forward_declare.h"

namespace preview {
namespace detail {

#if PREVIEW_CXX_VERSION >= 20
template<typename I, typename S>
struct is_specialized_iterator_traits<std::iterator_traits< std::common_iterator<I, S> >> : std::true_type {};

template<typename I, typename S>
struct is_specialized_iterator_traits<iterator_traits<std::common_iterator<I, S> >> : std::true_type {};
#endif

} // namespace detail
} // namespace preview

#endif // PREVIEW_ITERATOR_STD_COUNTED_ITERATOR_H_
