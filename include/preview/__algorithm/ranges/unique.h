//
// Created by yonggyulee on 03/02/2025
//

#ifndef PREVIEW_ALGORITHM_RANGES_UNIQUE_H_
#define PREVIEW_ALGORITHM_RANGES_UNIQUE_H_

#include <type_traits>

#include "preview/__algorithm/ranges/adjacent_find.h"
#include "preview/config.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/equal_to.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/indirect_equivalence_relation.h"
#include "preview/__iterator/iter_move.h"
#include "preview/__iterator/permutable.h"
#include "preview/__iterator/projected.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_subrange_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

struct unique_niebloid {
 private:
  template<typename I, typename Proj, typename Compare, bool = projectable<I, Proj>::value /* false */>
  struct projected_indirect_equivalence_relation : std::false_type {};
  template<typename I, typename Proj, typename Compare>
  struct projected_indirect_equivalence_relation<I, Proj, Compare, true>
      : indirect_equivalence_relation<Compare, projected<I, Proj>> {};

 public:
  template<typename I, typename S, class Proj = identity, typename Compare = ranges::equal_to, std::enable_if_t<conjunction_v<
      permutable<I>,
      sentinel_for<S, I>,
      projected_indirect_equivalence_relation<I, Proj, Compare>
  >, int> = 0>
  constexpr ranges::subrange<I> operator()(I first, S last, Compare comp = {}, Proj proj = {}) const {
#if !PREVIEW_CONFORM_CXX20_STANDARD
    using namespace preview::rel_ops;
#endif

    first = ranges::adjacent_find(first, last, preview::wrap_functor(comp), preview::wrap_functor(proj));
    if (first == last)
      return {first, first};

    I i{first};
    ++first;
    while (++first != last)
      if (!preview::invoke(comp, preview::invoke(proj, *i), preview::invoke(proj, *first)))
        *++i = ranges::iter_move(first);

    return {++i, first};
  }

  template<typename R, typename Proj = identity, typename Compare = ranges::equal_to, std::enable_if_t<conjunction_v<
      forward_range<R>,
      permutable<iterator_t<R>>,
      projected_indirect_equivalence_relation<iterator_t<R>, Proj, Compare>
  >, int> = 0>
  constexpr ranges::borrowed_subrange_t<R> operator()(R&& r, Compare comp = {}, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), preview::wrap_functor(comp), preview::wrap_functor(proj));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::unique_niebloid unique{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_UNIQUE_H_
