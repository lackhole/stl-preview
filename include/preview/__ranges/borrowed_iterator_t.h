//
// Created by yonggyulee on 2024/01/08.
//

#ifndef PREVIEW_RANGES_BORROWED_ITERATOR_T_H_
#define PREVIEW_RANGES_BORROWED_ITERATOR_T_H_

#include <type_traits>

#include "preview/__ranges/borrowed_range.h"
#include "preview/__ranges/dangling.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/range.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename R, bool v = range<R>::value /* true */>
struct borrowed_iterator_impl {
  using type = std::conditional_t<borrowed_range<R>::value, iterator_t<R>, dangling>;
};

template<typename R>
struct borrowed_iterator_impl<R, false> {};

} // namespace detail

template<typename R>
struct borrowed_iterator : detail::borrowed_iterator_impl<R> {};

template<typename R>
using borrowed_iterator_t = typename borrowed_iterator<R>::type;

} // namespace preview
} // namespace ranges

#endif // PREVIEW_RANGES_BORROWED_ITERATOR_T_H_
