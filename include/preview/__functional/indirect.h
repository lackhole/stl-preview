//
// Created by yonggyulee on 2024. 8. 14.
//

#ifndef PREVIEW_FUNCTIONAL_INDIRECT_H_
#define PREVIEW_FUNCTIONAL_INDIRECT_H_

#include <type_traits>
#include <utility>

#include "preview/__concepts/dereferenceable.h"

namespace preview {
namespace ranges {

struct indirect {
  template<typename T, std::enable_if_t<dereferenceable<T>::value, int> = 0>
  constexpr decltype(auto) operator()(T&& t) const noexcept(noexcept(*std::forward<T>(t))) {
    return *std::forward<T>(t);
  }
};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_FUNCTIONAL_INDIRECT_H_
