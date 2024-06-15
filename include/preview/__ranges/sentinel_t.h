//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_RANGES_SENTINEL_T_H_
#define PREVIEW_RANGES_SENTINEL_T_H_

#include <type_traits>

#include "preview/__ranges/end.h"
#include "preview/__ranges/range.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {

template<typename R, bool = range<R>::value, typename = void>
struct sentinel {};

template<typename R>
struct sentinel<R, true, void_t<decltype( ranges::end(std::declval<R&>()) )>> {
  using type = decltype(ranges::end(std::declval<R&>()));
};

template<typename R>
using sentinel_t = typename sentinel<R>::type;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_SENTINEL_T_H_
