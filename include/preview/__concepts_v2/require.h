//
// Created by yonggyulee on 2024. 9. 4.
//

#ifndef PREVIEW_CONCEPTS_V2_REQUIRE_H_
#define PREVIEW_CONCEPTS_V2_REQUIRE_H_

#include "preview/__concepts_v2/detail/constraints_not_satisfied.h"
#include "preview/__type_traits/is_invocable.h"

namespace preview {

template<typename T>
auto resolve_require(T x) {
  if constexpr (is_invocable<concepts::expand_error_fn, T>::value) {
    if constexpr (
        concepts::derived_from_template<T, concepts::constraints_not_satisfied>::value &&
        negation<is_specialization<T, concepts::constraints_not_satisfied>>::value)
    {
      return concepts::expand_error(concepts::constraints_not_satisfied<T, concepts::at<0, 1>>{});
    } else {
      return concepts::expand_error(x);
    }
  } else {
    return x;
  }
}

template<typename Error, std::size_t I, std::size_t N, typename... Reason>
auto resolve_require(concepts::constraints_not_satisfied<Error, concepts::at<I, N>, Reason...> x) { return x; }

} // namespace preview

#define preview_require(...) typename decltype(preview::resolve_require(__VA_ARGS__))::valid = true

#endif // PREVIEW_CONCEPTS_V2_REQUIRE_H_
