//
// Created by yonggyulee on 09/05/2025
//

#ifndef PREVIEW_ALGORITHM_RANGES_UNIQUE_COPY_H_
#define PREVIEW_ALGORITHM_RANGES_UNIQUE_COPY_H_

#include <type_traits>
#include <utility>

#include "preview/__algorithm/ranges/in_out_result.h"
#include "preview/__concepts/same_as.h"
#include "preview/__core/inline_variable.h"
#include "preview/__functional/equal_to.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/indirect_equivalence_relation.h"
#include "preview/__iterator/indirectly_copyable.h"
#include "preview/__iterator/indirectly_copyable_storable.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__iterator/projected.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__iterator/weakly_incrementable.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/detail/tag.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__utility/cxx20_rel_ops.h"
#include "preview/config.h"

namespace preview {
namespace ranges {

template<typename I, typename O>
using unique_copy_result = in_out_result<I, O>;

namespace detail {

struct unique_copy_niebloid {
 private:
  template<typename I, typename O, bool = /* false */ conjunction_v<
      input_iterator<O>,
      has_typename_type<iter_value<I>>,
      has_typename_type<iter_value<O>>
  >>
  struct can_reread_from_output : std::false_type {};
  template<typename I, typename O>
  struct can_reread_from_output<I, O, true> : same_as<iter_value_t<I>, iter_value_t<O>> {};

  template<typename I, typename S, typename O, typename Proj, typename Comp>
  constexpr unique_copy_result<I, O> call(I first, S last, O result, Comp comp, Proj proj, preview::detail::tag_1 /* reread first */) const {
#if !PREVIEW_CONFORM_CXX20_STANDARD
    using namespace preview::rel_ops;
#endif
    if (first != last) {
      I i = first;
      *result = *i;
      ++result;
      while (++first != last) {
        if (!preview::invoke(comp, preview::invoke(proj, *i), preview::invoke(proj, *first))) {
          *result = *first;
          ++result;
          i = first;
        }
      }
    }
    return {std::move(first), std::move(result)};
  }
  template<typename I, typename S, typename O, typename Proj, typename Comp>
  constexpr unique_copy_result<I, O> call(I first, S last, O result, Comp comp, Proj proj, preview::detail::tag_2 /* reread result */) const {
#if !PREVIEW_CONFORM_CXX20_STANDARD
    using namespace preview::rel_ops;
#endif
    if (first != last) {
      *result = *first;
      while (++first != last)
        if (!preview::invoke(comp, preview::invoke(proj, *result), preview::invoke(proj, *first))) {
          ++result;
          *result = *first;
        }
      ++result;
    }
    return {std::move(first), std::move(result)};
  }
  template<typename I, typename S, typename O, typename Proj, typename Comp>
  constexpr unique_copy_result<I, O> call(I first, S last, O result, Comp comp, Proj proj, preview::detail::tag_else /* use temp */) const {
#if !PREVIEW_CONFORM_CXX20_STANDARD
    using namespace preview::rel_ops;
#endif
    if (first != last) {
      iter_value_t<I> temp = *first;
      *result = std::move(temp);
      ++result;
      while (++first != last) {
        if (!preview::invoke(comp, preview::invoke(proj, temp), preview::invoke(proj, *first))) {
          temp = *first;
          *result = std::move(temp);
          ++result;
        }
      }
    }
    return {std::move(first), std::move(result)};
  }

 public:
  template<typename I, typename S, typename O,
           typename Proj = identity,
           typename Comp = ranges::equal_to,
    std::enable_if_t<conjunction_v<
      input_iterator<I>,
      sentinel_for<S, I>,
      weakly_incrementable<O>,
      projectable<I, Proj>,
      indirect_equivalence_relation<Comp, projected<I, Proj>>,
      // requires
      indirectly_copyable<I, O>,
      disjunction<
          forward_iterator<I>,
          can_reread_from_output<I, O>,
          indirectly_copyable_storable<I, O>
      >
    >, int> = 0
  >
  constexpr unique_copy_result<I, O> operator()(I first, S last, O result, Comp comp = {}, Proj proj = {}) const
      // precondition: [first, last) and [result, result + (last - first)) does not overlap
  {
#if PREVIEW_CONFORM_CXX20_STANDARD
    using namespace preview::rel_ops;
#endif
    using tag = preview::detail::conditional_tag<
        forward_iterator<I>,
        can_reread_from_output<I, O>
    >;
    return call(std::move(first), std::move(last), std::move(result),
        preview::wrap_functor(comp), preview::wrap_functor(proj), tag{});
  }

  template<typename R, typename O,
           typename Proj = identity,
           typename Comp = ranges::equal_to,
    std::enable_if_t<conjunction_v<
    >, int> = 0
  >
  constexpr unique_copy_result<borrowed_iterator_t<R>, O> operator()(R&& r, O result, Comp comp = {}, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), std::move(result), preview::wrap_functor(comp), preview::wrap_functor(proj));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::unique_copy_niebloid unique_copy{};

} // namespace ranges
} // namespace preview 

#endif // PREVIEW_ALGORITHM_RANGES_UNIQUE_COPY_H_
