//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_ITERATOR_UNREACHABLE_SENTINEL_H_
#define PREVIEW_ITERATOR_UNREACHABLE_SENTINEL_H_

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/weakly_incrementable.h"

namespace preview {

struct unreachable_sentinel_t {
  template<typename I>
  friend constexpr std::enable_if_t<weakly_incrementable<I>::value, bool>
  operator==(unreachable_sentinel_t, const I&) noexcept {
    return false;
  }

  template<typename I>
  friend constexpr std::enable_if_t<weakly_incrementable<I>::value, bool>
  operator==(const I&, unreachable_sentinel_t) noexcept {
    return false;
  }

  template<typename I>
  friend constexpr std::enable_if_t<weakly_incrementable<I>::value, bool>
  operator!=(unreachable_sentinel_t, const I&) noexcept {
    return false;
  }

  template<typename I>
  friend constexpr std::enable_if_t<weakly_incrementable<I>::value, bool>
  operator!=(const I&, unreachable_sentinel_t) noexcept {
    return false;
  }
};

PREVIEW_INLINE_VARIABLE constexpr unreachable_sentinel_t unreachable_sentinel{};

} // namespace preview

#endif // PREVIEW_ITERATOR_UNREACHABLE_SENTINEL_H_
