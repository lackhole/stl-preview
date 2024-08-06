//
// Created by yonggyulee on 2024. 8. 4.
//

#ifndef PREVIEW_ALGORITHM_RANGES_SEARCH_N_H_
#define PREVIEW_ALGORITHM_RANGES_SEARCH_N_H_

#include <functional>
#include <type_traits>
#include <utility>

#include "preview/core.h"
#include "preview/__algorithm/ranges/detail/advance_last.h"
#include "preview/__functional/equal_to.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/wrap_functor.h"
#include "preview/__iterator/advance.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/indirectly_comparable.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/projected_value_t.h"
#include "preview/__iterator/random_access_iterator.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_subrange_t.h"
#include "preview/__ranges/distance.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/from_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/size.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__ranges/subrange.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

struct search_n_niebloid {
  template<typename I, typename S, typename Pred = ranges::equal_to, typename Proj = identity, typename T = projected_value_t<I, Proj>,
      std::enable_if_t<conjunction_v<
          forward_iterator<I>,
          sentinel_for<S, I>,
          indirectly_comparable<I, const T*, Pred, Proj>
      >, int> = 0>
  constexpr subrange<I> operator()(I first, S last, iter_difference_t<I> count, const T& value, Pred pred = {}, Proj proj = {}) const {
    if (count <= 0)
      return {first, first};
    return call_iter(std::move(first), std::move(last), std::move(count), value, preview::wrap_functor(pred),
                     preview::wrap_functor(proj), conjunction<sized_sentinel_for<S, I>, random_access_iterator<I>>{});
  }

  template<typename R, typename Pred = ranges::equal_to, typename Proj = identity, typename T = projected_value_t<iterator_t<R>, Proj>,
      std::enable_if_t<conjunction_v<
          forward_range<R>,
          indirectly_comparable<iterator_t<R>, const T*, Pred, Proj>
      >, int> = 0>
  constexpr borrowed_subrange_t<R> operator()(R&& r, range_difference_t<R> count, const T& value, Pred pred = {}, Proj proj = {}) const {
    if (count <= 0) {
      auto first = ranges::begin(r);
      return {first, first};
    }
    return call_range(std::forward<R>(r), std::move(count), value, preview::wrap_functor(pred), preview::wrap_functor(proj), sized_range<R>{});
  }

 private:
  template<typename I, typename S, typename Pred, typename Proj, typename T>
  constexpr subrange<I> call_iter(I first, S last, iter_difference_t<I> count, const T& value, Pred pred, Proj proj,
                                  std::true_type /* sized_sentinel_for<S, I> && random_access_iterator<I> */) const
  {
    return search_sized(std::move(first), std::move(last), std::move(count), value,
                        preview::wrap_functor(pred), preview::wrap_functor(proj), ranges::distance(first, last));
  }
  template<typename I, typename S, typename Pred, typename Proj, typename T>
  constexpr subrange<I> call_iter(I first, S last, iter_difference_t<I> count, const T& value, Pred pred, Proj proj,
                                  std::false_type /* sized_sentinel_for<S, I> && random_access_iterator<I> */) const
  {
    return search(std::move(first), std::move(last), std::move(count), value, preview::wrap_functor(pred), preview::wrap_functor(proj));
  }

  template<typename R, typename Pred, typename Proj, typename T>
  constexpr borrowed_subrange_t<R> call_range(R&& r, range_difference_t<R> count, const T& value, Pred pred, Proj proj,
                                              std::true_type /* sized_range<R> */) const
  {
    return search_sized(ranges::begin(r), ranges::end(r), std::move(count), value,
                        preview::wrap_functor(pred), preview::wrap_functor(proj), ranges::size(r));
  }

  template<typename R, typename Pred, typename Proj, typename T>
  constexpr borrowed_subrange_t<R> call_range(R&& r, range_difference_t<R> count, const T& value, Pred pred, Proj proj,
                                              std::false_type /* sized_range<R> */) const
  {
    return search(ranges::begin(r), ranges::end(r), std::move(count), value, preview::wrap_functor(pred), preview::wrap_functor(proj));
  }

  template<typename I, typename S, typename Pred, typename Proj, typename T>
  constexpr subrange<I> search_sized(I first, S last, iter_difference_t<I> count, const T& value, Pred pred, Proj proj,
                                     iter_difference_t<I> distance) const
  {
    using namespace preview::rel_ops;

    for (; count <= distance; ++first, (void)--distance) {
      if (preview::invoke(pred, preview::invoke(proj, *first), value)) {
        I start = first;
        iter_difference_t<I> n = 0;

        for (;;) {
          ++first;
          if (++n == count)
            return {std::move(start), std::move(first)}; // found
          if (!preview::invoke(pred, preview::invoke(proj, *first), value))
            break; // not equal to value
        }
        distance -= n;
      }
    }

    ranges::detail::advance_last(first, std::move(last), distance);
    return {first, first};
  }

  template<typename I, typename S, typename Pred, typename Proj, typename T, typename Any>
  constexpr subrange<I> search(I first, S last, iter_difference_t<I> count, const T& value, Pred pred, Proj proj) const
  {
    using namespace preview::rel_ops;

    for (; first != last; ++first) {
      if (preview::invoke(pred, preview::invoke(proj, *first), value)) {
        I start = first;
        iter_difference_t<I> n = 0;

        for (;;) {
          ++first;
          if (n++ == count)
            return {std::move(start), std::move(first)}; // found
          if (first == last)
            return {first, first}; // not found
          if (!preview::invoke(pred, preview::invoke(proj, *first), value))
            break; // not equal to value
        }
      }
    }

    return {first, first};
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::search_n_niebloid search_n{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_SEARCH_N_H_
