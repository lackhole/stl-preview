//
// Created by yonggyulee on 2024. 9. 4.
//

#ifndef PREVIEW_CONCEPTS_V2_DETAIL_SIZED_TRUE_H_
#define PREVIEW_CONCEPTS_V2_DETAIL_SIZED_TRUE_H_

#include <cstddef>
#include <type_traits>

#include "preview/__concepts_v2/detail/constraints_not_satisfied.h"

namespace preview {
namespace concepts {

template<std::size_t N>
struct sized_true : std::true_type {
  using valid = bool;

  // true && true
  template<std::size_t N2>
  friend constexpr sized_true<N + N2> operator&&(sized_true, sized_true<N2>) noexcept { return {}; }

  // true && false
  template<typename Error, std::size_t I, std::size_t N2, typename... Info>
  friend constexpr auto operator&&(sized_true, constraints_not_satisfied<Error, at<I, N2>, Info...>) noexcept {
    return constraints_not_satisfied<Error, at<N + I, N + N2>, Info...>{};
  }

  // false && true
  template<typename Error, std::size_t I, std::size_t N2, typename... Info>
  friend constexpr auto operator&&(constraints_not_satisfied<Error, at<I, N2>, Info...>, sized_true) noexcept {
    return constraints_not_satisfied<Error, at<I, N + N2>, Info...>{};
  }

  // true || true
  template<std::size_t N2>
  friend constexpr sized_true<N + N2> operator||(sized_true, sized_true<N2>) noexcept { return {}; }

  // true || false
  template<typename Error, std::size_t I, std::size_t N2, typename... Info>
  friend constexpr sized_true<N + N2> operator||(sized_true, constraints_not_satisfied<Error, at<I, N2>, Info...>) noexcept { return {}; }

  // false || true
  template<typename Error, std::size_t I, std::size_t N2, typename... Info>
  friend constexpr sized_true<N + N2> operator||(constraints_not_satisfied<Error, at<I, N2>, Info...>, sized_true) noexcept { return {}; }


  // TODO: Preserve type when negating?
  // negation
  friend constexpr constraints_not_satisfied<sized_true, at<N, N>> operator!(sized_true) noexcept { return {}; }
};

// negation of constraints_not_satisfied
template<typename E, std::size_t I, std::size_t N, typename...Info>
constexpr sized_true<N> operator!(constraints_not_satisfied<E, at<I, N>, Info...>) noexcept { return {}; }

} // namespace concepts
} // namespace preview

#endif // PREVIEW_CONCEPTS_V2_DETAIL_SIZED_TRUE_H_
