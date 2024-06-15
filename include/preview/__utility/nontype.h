//
// Created by yonggyulee on 2024/01/02.
//

#ifndef PREVIEW_UTILITY_NONTYPE_H_
#define PREVIEW_UTILITY_NONTYPE_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"

namespace preview {

template<typename V>
struct nontype_t {
  constexpr explicit nontype_t() = default;
};

template<typename T>
PREVIEW_INLINE_VARIABLE constexpr nontype_t<T> nontype{};

} // namespace preview

#endif // PREVIEW_UTILITY_NONTYPE_H_
