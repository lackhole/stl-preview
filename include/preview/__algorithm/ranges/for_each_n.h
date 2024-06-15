//
// Created by yonggyulee on 1/26/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_IN_FOR_EACH_N_H_
#define PREVIEW_ALGORITHM_RANGES_IN_FOR_EACH_N_H_

#include <functional>
#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__algorithm/ranges/in_fun_result.h"
#include "preview/__functional/identity.h"
#include "preview/__functional/invoke.h"
#include "preview/__iterator/indirectly_unary_invocable.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/projected.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {

template<typename I, typename F>
using for_each_n_result = in_fun_result<I, F>;

namespace detail {

struct for_each_n_niebloid {
 private:
  template<typename I, typename Proj, typename Fun, bool = projectable<I, Proj>::value /* false */>
  struct check : std::false_type {};
  template<typename I, typename Proj, typename Fun>
  struct check<I, Proj, Fun, true> : indirectly_unary_invocable<Fun, projected<I, Proj>> {};

 public:
  template<typename I, typename Proj = identity, typename Fun, std::enable_if_t<conjunction<
      input_iterator<I>,
      check<I, Proj, Fun>
  >::value, int> = 0>
  constexpr for_each_n_result<I, Fun>
  operator()(I first, iter_difference_t<I> n, Fun f, Proj proj = {}) const {
    for (; n-- > 0; ++first) {
      preview::invoke(f, preview::invoke(proj, *first));
    }
    return {std::move(first), std::move(f)};
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::for_each_n_niebloid for_each_n{};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_IN_FOR_EACH_N_H_
