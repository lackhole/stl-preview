//
// Created by yonggyulee on 2024/01/08.
//

#ifndef PREVIEW_RANGES_BORROWED_SUBRANGE_T_H_
#define PREVIEW_RANGES_BORROWED_SUBRANGE_T_H_

#include <type_traits>

#include "preview/__ranges/borrowed_range.h"
#include "preview/__ranges/dangling.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/subrange.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename R, bool v = range<R>::value /* true */>
struct borrowed_subrange_impl {
  using type = std::conditional_t<borrowed_range<R>::value, subrange<iterator_t<R>>, dangling>;
};

template<typename R>
struct borrowed_subrange_impl<R, false> {};

} // namespace detail

template<typename R>
struct borrowed_subrange : detail::borrowed_subrange_impl<R> {};

template<typename R>
using borrowed_subrange_t = typename borrowed_subrange<R>::type;

} // namespace preview
} // namespace ranges

#endif // PREVIEW_RANGES_BORROWED_SUBRANGE_T_H_
