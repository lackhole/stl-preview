//
// Created by YongGyu Lee on 5/8/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_CLAMP_H_
#define PREVIEW_ALGORITHM_RANGES_CLAMP_H_

#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/less.h"
#include "preview/__iterator/indirect_strict_weak_order.h"
#include "preview/__iterator/projected.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace detail {

struct clamp_niebloid {
  template<typename T, typename Proj = identity, typename Comp = ranges::less, std::enable_if_t<conjunction<
      projectable<const T*, Proj>,
      indirect_strict_weak_order<Comp, projected<const T*, Proj>>
      >::value, int> = 0>
  constexpr const T& operator()(const T& v, const T& lo, const T& hi, Comp comp = {}, Proj proj = {}) const {
    auto&& pv = preview::invoke(proj, v);

    if (preview::invoke(comp, std::forward<decltype(pv)>(pv), preview::invoke(proj, lo)))
      return lo;

    if (preview::invoke(comp, preview::invoke(proj, hi), std::forward<decltype(pv)>(pv)))
      return hi;

    return v;
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::clamp_niebloid clamp{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_CLAMP_H_
