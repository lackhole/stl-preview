//
// Created by yonggyulee on 2023/12/30.
//

#ifndef PREVIEW_RANGES_SSIZE_H_
#define PREVIEW_RANGES_SSIZE_H_

#include <cstddef>
#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__ranges/size.h"
#include "preview/__type_traits/make_signed_like.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

struct ssize_niebloid {
  template<typename T, std::enable_if_t<is_invocable<decltype(ranges::size), T>::value, int> = 0>
  constexpr auto operator()(T&& t) const {
    using D1 = make_signed_like_t<decltype(ranges::size(t))>;
    using D2 = std::ptrdiff_t;
    using D = std::conditional_t<(sizeof(D1) > sizeof(D2)), D1, D2>;
    return static_cast<D>(ranges::size(t));
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::ssize_niebloid ssize{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_SSIZE_H_
