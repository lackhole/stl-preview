//
// Created by yonggyulee on 2024.12.01
//

#ifndef PREVIEW_ALGORITHM_RANGES_MINMAX_H_
#define PREVIEW_ALGORITHM_RANGES_MINMAX_H_

#include <initializer_list>
#include <type_traits>

#include "preview/__algorithm/ranges/min_max_result.h"
#include "preview/__algorithm/ranges/minmax_element.h"
#include "preview/__concepts/copyable.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/less.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/indirectly_copyable_storable.h"
#include "preview/__iterator/indirect_strict_weak_order.h"
#include "preview/__iterator/projected.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/range_value_t.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {

template<typename T>
using minmax_result = min_max_result<T>;

namespace detail {

struct minmax_niebloid {
 private:
  template<typename I, typename Proj, typename Comp, bool = /* false */ projectable<I, Proj>::value>
  struct check_range : std::false_type {};
  template<typename I, typename Proj, typename Comp>
  struct check_range<I, Proj, Comp, true> : indirect_strict_weak_order<Comp, projected<I, Proj>> {};

 public:
  template<typename T, typename Proj = identity, typename Comp = ranges::less, std::enable_if_t<
      indirect_strict_weak_order<Comp, projected<const T*, Proj>>::value, int> = 0>
  constexpr ranges::minmax_result<const T&>
  operator()(const T& a, const T& b, Comp comp = {}, Proj proj = {}) const {
    return preview::invoke(comp, preview::invoke(proj, b), preview::invoke(proj, a))
        ? ranges::minmax_result<const T&>{b, a}
        : ranges::minmax_result<const T&>{a, b};
  }

  template<typename T, typename Proj = identity, typename Comp = ranges::less, std::enable_if_t<conjunction_v<
      copyable<T>,
      check_range<const T*, Proj, Comp>>, int> = 0>
  constexpr ranges::minmax_result<const T&>
  operator()(std::initializer_list<T> il, Comp comp = {}, Proj proj = {}) const {
    auto result = ranges::minmax_element(il, preview::wrap_functor(comp), preview::wrap_functor(proj));
    return {*result.min, *result.max};
  }

  template<typename R, typename Proj = identity, typename Comp = ranges::less, std::enable_if_t<conjunction_v<
      input_range<R>,
      check_range<ranges::iterator_t<R>, Proj, Comp>,
      indirectly_copyable_storable<iterator_t<R>, range_value_t<R>*>
  >, int> = 0>
  constexpr ranges::minmax_result<range_value_t<R>>
  operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
    auto result = ranges::minmax_element(r, preview::wrap_functor(comp), preview::wrap_functor(proj));
    return {std::move(*result.min), std::move(*result.max)};
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::minmax_niebloid minmax{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_MINMAX_H_
