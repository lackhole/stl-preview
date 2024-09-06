//
// Created by yonggyulee on 2024. 9. 4.
//

#ifndef PREVIEW_CONCEPTS_V2_REQUIRE_H_
#define PREVIEW_CONCEPTS_V2_REQUIRE_H_

#include "preview/__concepts_v2/detail/constraints_not_satisfied.h"
#include "preview/__type_traits/is_invocable.h"

namespace preview {
namespace detail {

template<typename T>
constexpr auto resolve_require_2(T, std::true_type) {
  return concepts::expand_error(concepts::constraints_not_satisfied<T, concepts::at<0, 1>>{});
}

template<typename T>
constexpr auto resolve_require_2(T x, std::false_type) {
  return concepts::expand_error(x);
}

template<typename T>
constexpr auto resolve_require_1(T x, std::true_type) {
  return resolve_require_2(x, conjunction<
      concepts::derived_from_template<T, concepts::constraints_not_satisfied>,
      negation<is_specialization<T, concepts::constraints_not_satisfied>>>{});
}

template<typename T>
constexpr auto resolve_require_1(T x, std::false_type) {
  return x;
}

} // namespace detail

// T can be
// 1. a single concept
// 2. nested concepts that *already* evaluated to
//    2A. sized_true<N>
//    2B. constraints_not_satisfied
template<typename T>
constexpr auto resolve_require(T x) {
  return preview::detail::resolve_require_1(x, is_invocable<concepts::expand_error_fn, T>{});
}

template<typename Error, std::size_t I, std::size_t N, typename... Reason>
constexpr auto resolve_require(concepts::constraints_not_satisfied<Error, concepts::at<I, N>, Reason...> x) { return x; }

} // namespace preview

#define preview_require(...) typename decltype(preview::resolve_require(__VA_ARGS__))::valid = true

#endif // PREVIEW_CONCEPTS_V2_REQUIRE_H_
