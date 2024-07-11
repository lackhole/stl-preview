//
// Created by YongGyu Lee on 4/11/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_FIND_LAST_H_
#define PREVIEW_ALGORITHM_RANGES_FIND_LAST_H_

#include <functional>
#include <type_traits>

#include "preview/__algorithm/ranges/detail/check_input.h"
#include "preview/__core/inline_variable.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/indirect_binary_predicate.h"
#include "preview/__iterator/indirect_unary_predicate.h"
#include "preview/__iterator/next.h"
#include "preview/__iterator/projected.h"
#include "preview/__iterator/projected_value_t.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__functional/equal_to.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_subrange_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/subrange.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename I, typename S>
struct constant_last_gettable
    : disjunction<
        conjunction<
            bidirectional_iterator<I>,
            std::is_same<I, S>
        >,
        conjunction<
            random_access_iterator<I>,
            sized_sentinel_for<S, I>
        >
    > {};

template<typename I, typename S>
constexpr I get_last_iterator_impl(I, S last, std::true_type) {
  return last;
}

template<typename I, typename S>
constexpr I get_last_iterator_impl(I first, S last, std::false_type) {
  return first + ranges::distance(first, std::move(last));
}

template<typename I, typename S, std::enable_if_t<constant_last_gettable<I, S>::value, int> = 0>
constexpr I get_last_iterator(I first, S last) {
  return get_last_iterator_impl(std::move(first), std::move(last), conjunction<bidirectional_iterator<I>, std::is_same<I, S>>{});
}

struct find_last_niebloid {
 private:
  template<typename I, typename S, typename T, typename Proj>
  constexpr subrange<I> call(I first, S last, const T& value, Proj proj, std::true_type) const {
    using namespace rel_ops;

    I last_it = get_last_iterator(first, last);
    for (I out = last_it; out != first;) {
      if (preview::invoke(proj, *--out) == value)
        return {std::move(out), std::move(last_it)};
    }

    return {last_it, last_it};
  }

  template<typename I, typename S, typename T, typename Proj>
  constexpr subrange<I> call(I first, S last, const T& value, Proj proj, std::false_type) const {
    using namespace rel_ops;

    bool found = false;
    I out = first;

    for (;; ++first) {
      if (first == last) {
        if (!found)
          out = first; // last
        break;
      }
      if (preview::invoke(proj, *first) == value) {
        out = first;
        found = true;
      }
    }

    return {std::move(out), std::move(first /* == last */ )};
  }

 public:
  template<typename I, typename S, typename Proj = identity, typename T = projected_value_t<I, Proj>, std::enable_if_t<conjunction<
      forward_iterator<I>,
      sentinel_for<S, I>,
      projectable<I, Proj>,
      indirect_binary_predicate<equal_to, projected<I, Proj>, const T*>
  >::value, int> = 0>
  constexpr subrange<I> operator()(I first, S last, const T& value, Proj proj = {}) const {
    return call(std::move(first), std::move(last), value, std::ref(proj), constant_last_gettable<I, S>{});
  }

  template<typename R, typename Proj = identity, typename T = projected_value_t<iterator_t<R>, Proj>, std::enable_if_t<conjunction<
      input_range<R>,
      projectable<iterator_t<R>, Proj>,
      indirect_binary_predicate<equal_to, projected<iterator_t<R>, Proj>, const T*>
  >::value, int> = 0>
  constexpr borrowed_subrange_t<R> operator()(R&& r, const T& value, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), value, std::ref(proj));
  }
};

struct find_last_if_niebloid {
 private:
  template<typename I, typename S, typename Proj, typename Pred>
  constexpr subrange<I> call(I first, S last, Pred pred, Proj proj, std::true_type) const {
    using namespace rel_ops;

    I last_it = get_last_iterator(first, last);
    for (I out = last_it; out != first;) {
      if (preview::invoke(pred, preview::invoke(proj, *--out)))
        return {std::move(out), std::move(last_it)};
    }

    return {last_it, last_it};
  }

  template<typename I, typename S, typename Proj, typename Pred>
  constexpr subrange<I> call(I first, S last, Pred pred, Proj proj, std::false_type) const {
    using namespace rel_ops;

    bool found = false;
    I out = first;

    for (;; ++first) {
      if (first == last) {
        if (!found)
          out = first; // last
        break;
      }
      if (preview::invoke(pred, preview::invoke(proj, *first))) {
        out = first;
        found = true;
      }
    }

    return {std::move(out), std::move(first /* == last */ )};
  }

 public:
  template<typename I, typename S, typename Proj = identity, typename Pred, std::enable_if_t<conjunction<
      forward_iterator<I>,
      sentinel_for<S, I>,
      projectable<I, Proj>,
      indirect_unary_predicate<Pred, projected<I, Proj>>
  >::value, int> = 0>
  constexpr subrange<I> operator()(I first, S last, Pred pred, Proj proj = {}) const {
    return call(std::move(first), std::move(last), std::ref(pred), std::ref(proj), constant_last_gettable<I, S>{});
  }

  template<typename R, typename Proj = identity, typename Pred, std::enable_if_t<conjunction<
      forward_range<R>,
      projectable<iterator_t<R>, Proj>,
      indirect_unary_predicate<Pred, projected<iterator_t<R>, Proj>>
  >::value, int> = 0>
  constexpr borrowed_subrange_t<R> operator()(R&& r, Pred pred, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), std::ref(pred), std::ref(proj));
  }
};

struct find_last_if_not_niebloid {
 private:
  template<typename I, typename S, typename Proj, typename Pred>
  constexpr subrange<I> call(I first, S last, Pred pred, Proj proj, std::true_type) const {
    using namespace rel_ops;

    I last_it = get_last_iterator(first, last);
    for (I out = last_it; out != first;) {
      if (!preview::invoke(pred, preview::invoke(proj, *--out)))
        return {std::move(out), std::move(last_it)};
    }

    return {last_it, last_it};
  }

  template<typename I, typename S, typename Proj, typename Pred>
  constexpr subrange<I> call(I first, S last, Pred pred, Proj proj, std::false_type) const {
    using namespace rel_ops;

    bool found = false;
    I out = first;

    for (;; ++first) {
      if (first == last) {
        if (!found)
          out = first; // last
        break;
      }
      if (!preview::invoke(pred, preview::invoke(proj, *first))) {
        out = first;
        found = true;
      }
    }

    return {std::move(out), std::move(first /* == last */ )};
  }

 public:
  template<typename I, typename S, typename Proj = identity, typename Pred, std::enable_if_t<conjunction<
      forward_iterator<I>,
      sentinel_for<S, I>,
      projectable<I, Proj>,
      indirect_unary_predicate<Pred, projected<I, Proj>>
  >::value, int> = 0>
  constexpr subrange<I> operator()(I first, S last, Pred pred, Proj proj = {}) const {
    return call(std::move(first), std::move(last), std::ref(pred), std::ref(proj), constant_last_gettable<I, S>{});
  }

  template<typename R, typename Proj = identity, typename Pred, std::enable_if_t<conjunction<
      forward_range<R>,
      projectable<iterator_t<R>, Proj>,
      indirect_unary_predicate<Pred, projected<iterator_t<R>, Proj>>
  >::value, int> = 0>
  constexpr borrowed_subrange_t<R> operator()(R&& r, Pred pred, Proj proj = {}) const {
    return (*this)(ranges::begin(r), ranges::end(r), std::ref(pred), std::ref(proj));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::find_last_niebloid find_last{};
PREVIEW_INLINE_VARIABLE constexpr detail::find_last_if_niebloid find_last_if{};
PREVIEW_INLINE_VARIABLE constexpr detail::find_last_if_not_niebloid find_last_if_not{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_FIND_LAST_H_
