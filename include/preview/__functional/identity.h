//
// Created by YongGyu Lee on 12/28/23.
//

#ifndef PREVIEW_FUNCTIONAL_IDENTITY_H_
#define PREVIEW_FUNCTIONAL_IDENTITY_H_

#include <type_traits>
#include <utility>

namespace preview {

struct identity {
  template<typename T>
  constexpr T&& operator()(T&& t) const noexcept {
    return std::forward<T>(t);
  }

  using is_transparent = std::true_type;
};

} // namespace preview

#endif // PREVIEW_FUNCTIONAL_IDENTITY_H_
