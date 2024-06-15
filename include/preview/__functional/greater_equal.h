//
// Created by YongGyu Lee on 2024/02/04.
//

#ifndef PREVIEW_FUNCTIONAL_GREATER_EQUAL_H_
#define PREVIEW_FUNCTIONAL_GREATER_EQUAL_H_

#include <type_traits>
#include <utility>

#include "preview/__concepts/totally_ordered.h"
#include "preview/__functional/less.h"

namespace preview {
namespace ranges {

struct greater_equal {
  using is_transparent = std::true_type;

  template<typename T, typename U, std::enable_if_t<totally_ordered_with<T, U>::value, int> = 0>
  constexpr bool operator()(T&& t, U&& u) const
      noexcept(noexcept(!ranges::less{}(std::forward<T>(t), std::forward<U>(u))))
  {
    return !ranges::less{}(std::forward<T>(t), std::forward<U>(u));
  }
};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_FUNCTIONAL_GREATER_EQUAL_H_
