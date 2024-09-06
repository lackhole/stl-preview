//
// Created by yonggyulee on 2024. 9. 4.
//

#ifndef PREVIEW_CONCEPTS_V2_DETAIL_CONSTRAINTS_NOT_SATISFIED_H_
#define PREVIEW_CONCEPTS_V2_DETAIL_CONSTRAINTS_NOT_SATISFIED_H_

#include <cstddef>
#include <tuple>
#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__concepts_v2/detail/forward_declare.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/is_specialization.h"
#include "preview/__type_traits/negation.h"

namespace preview {
namespace concepts {

template<typename Constraints, typename... Info>
struct constraints_not_satisfied : std::false_type {};

template<typename Constraints, typename... OptionalInfo>
struct because;

// TODO: simplify nested constraints to `is_false<final_constraints, at<I, N>>`
//       instead of `constraints_not_satisfied<final_constraints, at<I, N>>`
struct is_false {};

struct concat_word {};

struct and_ : concat_word {};

template<std::size_t I, std::size_t N>
struct at {};

/// concat_error

template<typename CNS1, typename CNS2>
struct concat_error;

template<typename E1, typename... Info1, typename E2, typename... Info2>
struct concat_error<constraints_not_satisfied<E1, Info1...>, constraints_not_satisfied<E2, Info2...>> {
  using type = constraints_not_satisfied<E1, Info1..., and_, E2, Info2...>;
};

template<typename CNS1, typename CNS2>
using concat_error_t = typename concat_error<CNS1, CNS2>::type;

/// expand_error

// TODO: Optimize
struct expand_error_fn {
  template<typename Constraint, typename... Info>
  constexpr auto operator()(constraints_not_satisfied<Constraint, Info...> e) const noexcept {
    return call(e, is_invocable<expand_error_fn, Constraint>{});
  }

  // Ignore already expanded error
  template<typename Constraint, std::size_t I, std::size_t N, typename... Why, typename... Rest>
  constexpr auto operator()(constraints_not_satisfied<Constraint, at<I, N>, because<Why...>, Rest...> e) const noexcept {
    return e;
  }
  // This case doesn't exist yest
  template<typename Constraint, typename... Why, typename... Rest>
  constexpr auto operator()(constraints_not_satisfied<Constraint, because<Why...>, Rest...> e) const noexcept {
    return e;
  }

  // direct-wrapper of type_traits (e.g., integral = is_integral_v)
  template<typename Derived, typename Base, std::enable_if_t<conjunction<
      derived_from_bool_constant<Base>,
      negation<is_concept<Base>>,
      negation<derived_from_template<Base, constraints_not_satisfied>>
  >::value, int> = 0>
  constexpr auto operator()(concept_base<Derived, Base>) const noexcept {
    // TODO: Simplify
    return constraints_not_satisfied<Derived, at<0, 1>, because<Base, is_false>>{};
  }

 private:
  template<typename Error, typename... Info>
  constexpr auto call(constraints_not_satisfied<Error, Info...>, std::true_type) const noexcept {
    auto e = (*this)(Error{});
    return constraints_not_satisfied<Error, Info..., because<decltype(e)>>{};
  }
  template<typename Error, typename... Info>
  constexpr auto call(constraints_not_satisfied<Error, Info...> e, std::false_type) const noexcept {
    return e;
  }
};

PREVIEW_INLINE_VARIABLE constexpr expand_error_fn expand_error{};

template<std::size_t Ix, std::size_t Nx, typename CNS, typename OutTuple = std::tuple<>>
struct add_to_at;

template<std::size_t Ix, std::size_t Nx,
         typename C, std::size_t I, std::size_t N,
         typename... Args>
struct add_to_at<Ix, Nx, constraints_not_satisfied<C, at<I, N>>, std::tuple<Args...>> {
  using type = constraints_not_satisfied<Args..., C, at<I + Ix, N + Nx>>;
};

template<std::size_t Ix, std::size_t Nx,
         typename C, std::size_t I, std::size_t N, typename... Reason,
         typename... Args>
struct add_to_at<Ix, Nx, constraints_not_satisfied<C, at<I, N>, because<Reason...>>, std::tuple<Args...>> {
  using type = constraints_not_satisfied<Args..., C, at<I + Ix, N + Nx>, because<Reason...>>;
};

template<std::size_t Ix, std::size_t Nx,
    typename C, std::size_t I, std::size_t N, typename... Rest,
    typename... Args>
struct add_to_at<Ix, Nx, constraints_not_satisfied<C, at<I, N>, Rest...>, std::tuple<Args...>> {
  using type = typename add_to_at<
      Ix, Nx,
      constraints_not_satisfied<Rest...>,
      std::tuple<Args..., C, at<I + Ix, N + Nx>>
  >::type;
};

template<std::size_t Ix, std::size_t Nx,
         typename C, std::size_t I, std::size_t N, typename... Rest,
         typename... Args>
struct add_to_at<Ix, Nx, constraints_not_satisfied<C, at<I, N>, and_, Rest...>, std::tuple<Args...>> {
  using type = typename add_to_at<
      Ix, Nx,
      constraints_not_satisfied<Rest...>,
      std::tuple<Args..., C, at<I + Ix, N + Nx>, and_>
  >::type;
};

template<std::size_t Ix, std::size_t Nx,
         typename C, std::size_t I, std::size_t N, typename... Reason, typename... Rest,
         typename... Args>
struct add_to_at<Ix, Nx, constraints_not_satisfied<C, at<I, N>, because<Reason...>, and_, Rest...>, std::tuple<Args...>> {
  using type = typename add_to_at<
      Ix, Nx,
      constraints_not_satisfied<Rest...>,
      std::tuple<Args..., C, at<I + Ix, N + Nx>, because<Reason...>, and_>
  >::type;
};

template<std::size_t Ix, std::size_t Nx, typename CNS, typename OutTuple = std::tuple<>>
using add_to_at_t = typename add_to_at<Ix, Nx, CNS, OutTuple>::type;

// TODO: hide from global
template<typename E1, std::size_t I1, std::size_t N1, typename...Info1, typename E2, std::size_t I2, std::size_t N2, typename... Info2>
constexpr auto operator&&(constraints_not_satisfied<E1, at<I1, N1>, Info1...> e, constraints_not_satisfied<E2, at<I2, N2>, Info2...>) noexcept
    { return add_to_at_t<0, N2, decltype(e)>{}; }

template<typename E1, std::size_t I1, std::size_t N1, typename...Info1, typename E2, std::size_t I2, std::size_t N2, typename... Info2>
constexpr auto operator||(constraints_not_satisfied<E1, at<I1, N1>, Info1...> e1, constraints_not_satisfied<E2, at<I2, N2>, Info2...> e2) noexcept {
  using LE = add_to_at_t<0, N2, decltype(e1)>;
  using RE = add_to_at_t<N1, N1, decltype(e2)>;
  return concat_error_t<LE, RE>{};
}

// negation is defined under sized_true.h


/// Instantiation is required to print the type of the parameter pack in the error message on some compilers

template<typename Constraints>
struct because<Constraints> : Constraints {};
template<typename C, typename I1>
struct because<C, I1> : C {};
template<typename C, typename I1, typename I2>
struct because<C, I1, I2> : C {};
template<typename C, typename I1, typename I2, typename I3>
struct because<C, I1, I2, I3> : C {};
template<typename C, typename I1, typename I2, typename I3, typename I4>
struct because<C, I1, I2, I3, I4> : C {};
template<typename C, typename I1, typename I2, typename I3, typename I4, typename I5>
struct because<C, I1, I2, I3, I4, I5> : C {};
template<typename C, typename I1, typename I2, typename I3, typename I4, typename I5, typename I6>
struct because<C, I1, I2, I3, I4, I5, I6> : C {};
template<typename C, typename I1, typename I2, typename I3, typename I4, typename I5, typename I6, typename I7>
struct because<C, I1, I2, I3, I4, I5, I6, I7> : C {};
template<typename C, typename I1, typename I2, typename I3, typename I4, typename I5, typename I6, typename I7, typename I8>
struct because<C, I1, I2, I3, I4, I5, I6, I7, I8> : C {};
template<typename C, typename I1, typename I2, typename I3, typename I4, typename I5, typename I6, typename I7, typename I8, typename I9>
struct because<C, I1, I2, I3, I4, I5, I6, I7, I8, I9> : C {};
template<typename C, typename I1, typename I2, typename I3, typename I4, typename I5, typename I6, typename I7, typename I8, typename I9, typename... Is>
struct because<C, I1, I2, I3, I4, I5, I6, I7, I8, I9, Is...> : C {};

// Single fail
template<typename C1, typename... Why,
         typename... Rest>
struct constraints_not_satisfied<
    C1, because<Why...>,
    Rest...> : std::false_type {};

// Single fail with location
template<typename C1, std::size_t I, std::size_t N, typename... Why,
         typename... Rest>
struct constraints_not_satisfied<
    C1, at<I, N>, because<Why...>,
    Rest... > : std::false_type {};

// Multiple fail with 2 disjunction
template<typename C1, std::size_t I1, std::size_t N1,
         typename C2, std::size_t I2, std::size_t N2,
         typename... Rest>
struct constraints_not_satisfied<
          C1, at<I1, N1>,
    and_, C2, at<I2, N2>,
    Rest... > : std::false_type {};

template<typename C1, std::size_t I1, std::size_t N1, typename... Why1,
         typename C2, std::size_t I2, std::size_t N2,
         typename... Rest>
struct constraints_not_satisfied<
          C1, at<I1, N1>, because<Why1...>,
    and_, C2, at<I2, N2>,
    Rest... > : std::false_type {};

template<typename C1, std::size_t I1, std::size_t N1,
         typename C2, std::size_t I2, std::size_t N2, typename... Why2,
         typename... Rest>
struct constraints_not_satisfied<
          C1, at<I1, N1>,
    and_, C2, at<I2, N2>, because<Why2...>,
    Rest... > : std::false_type {};

template<typename C1, std::size_t I1, std::size_t N1, typename... Why1,
         typename C2, std::size_t I2, std::size_t N2, typename... Why2,
         typename... Rest>
struct constraints_not_satisfied<
          C1, at<I1, N1>, because<Why1...>,
    and_, C2, at<I2, N2>, because<Why2...>,
    Rest... > : std::false_type {};

// Multiple fail with 3 disjunction
template<typename C1, std::size_t I1, std::size_t N1,
         typename C2, std::size_t I2, std::size_t N2,
         typename C3, std::size_t I3, std::size_t N3,
         typename... Rest>
struct constraints_not_satisfied<
          C1, at<I1, N1>,
    and_, C2, at<I2, N2>,
    and_, C3, at<I3, N3>,
    Rest... > : std::false_type {};

template<typename C1, std::size_t I1, std::size_t N1,
         typename C2, std::size_t I2, std::size_t N2,
         typename C3, std::size_t I3, std::size_t N3, typename... Why3,
         typename... Rest>
struct constraints_not_satisfied<
          C1, at<I1, N1>,
    and_, C2, at<I2, N2>,
    and_, C3, at<I3, N3>, because<Why3...>,
    Rest... > : std::false_type {};

template<typename C1, std::size_t I1, std::size_t N1,
         typename C2, std::size_t I2, std::size_t N2, typename... Why2,
         typename C3, std::size_t I3, std::size_t N3,
         typename... Rest>
struct constraints_not_satisfied<
          C1, at<I1, N1>,
    and_, C2, at<I2, N2>, because<Why2...>,
    and_, C3, at<I3, N3>,
    Rest... > : std::false_type {};

template<typename C1, std::size_t I1, std::size_t N1,
         typename C2, std::size_t I2, std::size_t N2, typename... Why2,
         typename C3, std::size_t I3, std::size_t N3, typename... Why3,
         typename... Rest>
struct constraints_not_satisfied<
          C1, at<I1, N1>,
    and_, C2, at<I2, N2>, because<Why2...>,
    and_, C3, at<I3, N3>, because<Why3...>,
    Rest... > : std::false_type {};

template<typename C1, std::size_t I1, std::size_t N1, typename... Why1,
         typename C2, std::size_t I2, std::size_t N2,
         typename C3, std::size_t I3, std::size_t N3,
         typename... Rest>
struct constraints_not_satisfied<
          C1, at<I1, N1>, because<Why1...>,
    and_, C2, at<I2, N2>,
    and_, C3, at<I3, N3>,
    Rest... > : std::false_type {};

template<typename C1, std::size_t I1, std::size_t N1, typename... Why1,
         typename C2, std::size_t I2, std::size_t N2,
         typename C3, std::size_t I3, std::size_t N3, typename... Why3,
         typename... Rest>
struct constraints_not_satisfied<
          C1, at<I1, N1>, because<Why1...>,
    and_, C2, at<I2, N2>,
    and_, C3, at<I3, N3>, because<Why3...>,
    Rest... > : std::false_type {};

template<typename C1, std::size_t I1, std::size_t N1, typename... Why1,
         typename C2, std::size_t I2, std::size_t N2, typename... Why2,
         typename C3, std::size_t I3, std::size_t N3,
         typename... Rest>
struct constraints_not_satisfied<
          C1, at<I1, N1>, because<Why1...>,
    and_, C2, at<I2, N2>, because<Why2...>,
    and_, C3, at<I3, N3>,
    Rest... > : std::false_type {};

template<typename C1, std::size_t I1, std::size_t N1, typename... Why1,
         typename C2, std::size_t I2, std::size_t N2, typename... Why2,
         typename C3, std::size_t I3, std::size_t N3, typename... Why3,
         typename... Rest>
struct constraints_not_satisfied<
          C1, at<I1, N1>, because<Why1...>,
    and_, C2, at<I2, N2>, because<Why2...>,
    and_, C3, at<I3, N3>, because<Why3...>,
    Rest... > : std::false_type {};

} // namespace concepts
} // namespace preview

#endif // PREVIEW_CONCEPTS_V2_DETAIL_CONSTRAINTS_NOT_SATISFIED_H_
