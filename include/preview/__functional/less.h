//
// Created by YongGyu Lee on 2024/01/03.
//

#ifndef PREVIEW_FUNCTIONAL_LESS_H_
#define PREVIEW_FUNCTIONAL_LESS_H_

#include <type_traits>
#include <utility>

#include "preview/__concepts/totally_ordered.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {

struct less {
  using is_transparent = std::true_type;

  template<typename T, typename U, std::enable_if_t<totally_ordered_with<T, U>::value, int> = 0>
  constexpr bool operator()(T&& t, U&& u) const
      noexcept(noexcept(bool(std::forward<T>(t) < std::forward<U>(u))))
  {
    using namespace preview::rel_ops;
    return std::forward<T>(t) < std::forward<U>(u);
  }
};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_FUNCTIONAL_LESS_H_
