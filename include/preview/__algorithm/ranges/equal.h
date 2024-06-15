//
// Created by yonggyulee on 1/12/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_EQUAL_H_
#define PREVIEW_ALGORITHM_RANGES_EQUAL_H_

#include <functional>
#include <initializer_list>
#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__core/nodiscard.h"
#include "preview/__iterator/indirectly_comparable.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__functional/equal_to.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/distance.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace detail {

struct equal_niebloid {
 private:
  template<typename R1, typename R2, typename Pred, typename Proj1, typename Proj2,
      bool = conjunction<input_range<R1>, input_range<R2>>::value /* false */>
  struct check_range : std::false_type {};
  template<typename R1, typename R2, typename Pred, typename Proj1, typename Proj2>
  struct check_range<R1, R2, Pred, Proj1, Proj2, true>
      : indirectly_comparable<iterator_t<R1>, iterator_t<R2>, Pred, Proj1, Proj2> {};

  template<typename I1, typename S1, typename I2, typename S2>
  constexpr bool compare_size(I1 first1, S1 last1, I2 first2, S2 last2, std::true_type /* sized_sentinel_for */) const {
    return ranges::distance(first1, last1) == ranges::distance(first2, last2);
  }
  template<typename I1, typename S1, typename I2, typename S2>
  constexpr bool compare_size(I1, S1, I2, S2, std::false_type /* sized_sentinel_for */) const {
    return true;
  }

 public:
  template<
      typename I1, typename S1,
      typename I2, typename S2,
      typename Pred = ranges::equal_to,
      typename Proj1 = identity, typename Proj2 = identity,
      std::enable_if_t<conjunction<
          input_iterator<I1>, sentinel_for<S1, I1>,
          input_iterator<I2>, sentinel_for<S2, I2>,
          indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
      >::value, int> = 0
  >
  PREVIEW_NODISCARD constexpr bool
  operator()(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    if (!compare_size(first1, last1, first2, last2, conjunction<sized_sentinel_for<S1, I1>, sized_sentinel_for<S2, I2>>{}))
      return false;

    for (; first1 != last1; ++first1, (void)++first2) {
      if (!preview::invoke(pred, preview::invoke(proj1, *first1), preview::invoke(proj2, *first2)))
        return false;
    }
    return true;
  }

  template<
      typename R1,
      typename R2,
      typename Pred = ranges::equal_to,
      typename Proj1 = preview::identity, typename Proj2 = preview::identity,
      std::enable_if_t<check_range<R1, R2, Pred, Proj1, Proj2>::value, int> = 0
  >
  PREVIEW_NODISCARD constexpr bool
  operator()(R1&& r1, R2&& r2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    return (*this)(ranges::begin(r1), ranges::end(r1), ranges::begin(r2), ranges::end(r2),
                   std::ref(pred), std::ref(proj1), std::ref(proj2));
  }

  template<
      typename T,
      typename R2,
      typename Pred = equal_to,
      typename Proj1 = identity, typename Proj2 = identity,
      std::enable_if_t<check_range<std::initializer_list<T>, R2, Pred, Proj1, Proj2>::value, int> = 0
  >
  PREVIEW_NODISCARD constexpr bool
  operator()(std::initializer_list<T>&& il, R2&& r2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    return (*this)(ranges::begin(il), ranges::end(il), ranges::begin(r2), ranges::end(r2),
                   std::ref(pred), std::ref(proj1), std::ref(proj2));
  }

  template<
      typename R1,
      typename U,
      typename Pred = equal_to,
      typename Proj1 = identity, typename Proj2 = identity,
      std::enable_if_t<check_range<R1, std::initializer_list<U>, Pred, Proj1, Proj2>::value, int> = 0
  >
  PREVIEW_NODISCARD constexpr bool
  operator()(R1&& r1, std::initializer_list<U> il, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    return (*this)(ranges::begin(r1), ranges::end(r1), ranges::begin(il), ranges::end(il),
                   std::ref(pred), std::ref(proj1), std::ref(proj2));
  }

  template<
      typename T,
      typename U,
      typename Pred = equal_to,
      typename Proj1 = identity, typename Proj2 = identity,
      std::enable_if_t<check_range<std::initializer_list<T>, std::initializer_list<U>, Pred, Proj1, Proj2>::value, int> = 0
  >
  PREVIEW_NODISCARD constexpr bool
  operator()(std::initializer_list<T> il1, std::initializer_list<U> il2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const {
    return (*this)(ranges::begin(il1), ranges::end(il1), ranges::begin(il2), ranges::end(il2),
                   std::ref(pred), std::ref(proj1), std::ref(proj2));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::equal_niebloid equal{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_EQUAL_H_
