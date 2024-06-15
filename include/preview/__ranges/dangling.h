//
// Created by yonggyulee on 2023/12/29.
//

#ifndef PREVIEW_RANGES_DANGLING_H_
#define PREVIEW_RANGES_DANGLING_H_

namespace preview {
namespace ranges {

struct dangling {
  constexpr dangling() noexcept = default;

  template<typename... Args>
  constexpr dangling(Args&&...) noexcept {}
};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_DANGLING_H_
