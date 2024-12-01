//
// Created by yonggyulee on 2024.12.01
//

#ifndef PREVIEW_ALGORITHM_RANGES_MINMAX_ELEMENT_H_
#define PREVIEW_ALGORITHM_RANGES_MINMAX_ELEMENT_H_

#include <type_traits>

#include "preview/__algorithm/ranges/min_max_result.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/less.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/indirect_strict_weak_order.h"
#include "preview/__iterator/projected.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {

template<typename I>
using minmax_element_result = min_max_result<I>;

namespace detail {

struct minmax_element_niebloid {
 private:
  template<typename I, typename Proj, typename Comp, bool = /* false */ projectable<I, Proj>::value>
  struct check_range : std::false_type {};
  template<typename I, typename Proj, typename Comp>
  struct check_range<I, Proj, Comp, true> : indirect_strict_weak_order<Comp, projected<I, Proj>> {};

  // lambda is not constexpr until C++17
  template<typename Proj, typename Comp>
  struct minmax_element_less {
    Proj& proj;
    Comp& comp;

    template<typename I>
    constexpr auto operator()(const I& x, const I& y) const {
      return preview::invoke(comp, preview::invoke(proj, *x), preview::invoke(proj, *y));
    }
  };

 public:
  template<typename I, typename S, typename Proj = identity, typename Comp = ranges::less, std::enable_if_t<conjunction<
      forward_iterator<I>,
      sentinel_for<S, I>,
      check_range<I, Proj, Comp>
  >::value, int> = 0>
  constexpr minmax_element_result<I> operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
    using namespace preview::rel_ops;

    I min_ = first;
    I max_ = first;
    if (first == last || ++first == last) {
      return {min_, max_};
    }

    minmax_element_less<Proj, Comp> less{proj, comp};

    if (less(first, min_))
      min_ = first;
    else
      max_ = first;

    while (++first != last) {
      I i = first;
      if (++first == last) {
        if (less(i, min_))
          min_ = i;
        else if (!less(i, max_))
          max_ = i;
        break;
      }

      if (less(first, i)) {
        if (less(first, min_))
          min_ = first;
        if (!less(i, max_))
          max_ = i;
      } else {
        if (less(i, min_))
          min_ = i;
        if (!less(first, max_))
          max_ = first;
      }
    }

    return {min_, max_};
  }

  template<typename R, typename Proj = identity, typename Comp = ranges::less, std::enable_if_t<conjunction<
      forward_range<R>,
      check_range<iterator_t<R>, Proj, Comp>
  >::value, int> = 0>
  constexpr minmax_element_result<borrowed_iterator_t<R>> operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), preview::wrap_functor(comp), preview::wrap_functor(proj));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::minmax_element_niebloid minmax_element{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_MINMAX_ELEMENT_H_
