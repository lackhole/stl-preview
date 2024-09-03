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
struct constraints_not_satisfied;

template<typename Constraints, typename... OptionalInfo>
struct because : Constraints {};

// TODO: simplify nested constraints to `is_false<final_constraints, at<I, N>>`
//       instead of `constraints_not_satisfied<final_constraints, at<I, N>>`
struct is_false {};

struct concat_word {};

struct and_ : concat_word {};

template<std::size_t I, std::size_t N>
struct at {};

// Default type
template<typename Error, std::size_t I, std::size_t N, typename... Reason>
struct constraints_not_satisfied<Error, at<I, N>, Reason...> : std::false_type {};

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

//  template<typename Error, std::enable_if_t<conjunction<
//      derived_from_template<Error, constraints_not_satisfied>,
//      negation<is_specialization<Error, constraints_not_satisfied>>
//  >::value, int> = 0>
//  constexpr auto operator()(Error) const noexcept {
//    return (*this)(constraints_not_satisfied<Error, at<0, 0>>{});
//  }

  // direct-wrapper of type_traits (e.g., integral = is_integral_v)
  template<typename Derived, typename Base, std::enable_if_t<conjunction<
      disjunction<
        std::is_base_of<std::integral_constant<bool, true>, Base>,
        std::is_base_of<std::integral_constant<bool, false>, Base>
      >,
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
    typename C, std::size_t I, std::size_t N, typename Reason,
    typename... Args>
struct add_to_at<Ix, Nx, constraints_not_satisfied<C, at<I, N>, because<Reason>>, std::tuple<Args...>> {
  using type = constraints_not_satisfied<Args..., C, at<I + Ix, N + Nx>, because<Reason>>;
};

template<std::size_t Ix, std::size_t Nx,
    typename C, std::size_t I, std::size_t N, typename... Rest,
    typename... Args>
struct add_to_at<Ix, Nx, constraints_not_satisfied<C, at<I, N>, and_, Rest...>, std::tuple<Args...>> {
  using type = typename add_to_at<Ix, Nx, constraints_not_satisfied<Rest...>, std::tuple<Args..., C, at<I + Ix, N + Nx>, and_>>::type;
};

template<std::size_t Ix, std::size_t Nx,
    typename C, std::size_t I, std::size_t N, typename Reason, typename... Rest,
    typename... Args>
struct add_to_at<Ix, Nx, constraints_not_satisfied<C, at<I, N>, because<Reason>, and_, Rest...>, std::tuple<Args...>> {
  using type = typename add_to_at<Ix, Nx, constraints_not_satisfied<Rest...>, std::tuple<Args..., C, at<I + Ix, N + Nx>, and_>>::type;
};

template<std::size_t Ix, std::size_t Nx, typename CNS, typename OutTuple = std::tuple<>>
using add_to_at_t = typename add_to_at<Ix, Nx, CNS, OutTuple>::type;

// TODO: hide from global
template<typename E1, std::size_t I1, std::size_t N1, typename...Info1, typename E2, std::size_t I2, std::size_t N2, typename... Info2>
constexpr auto operator&&(constraints_not_satisfied<E1, at<I1, N1>, Info1...> e, constraints_not_satisfied<E2, at<I2, N2>, Info2...>) noexcept {
  add_to_at_t<0, N2, decltype(e)>{};
}

template<typename E1, std::size_t I1, std::size_t N1, typename...Info1, typename E2, std::size_t I2, std::size_t N2, typename... Info2>
constexpr auto operator||(constraints_not_satisfied<E1, at<I1, N1>, Info1...> e1, constraints_not_satisfied<E2, at<I2, N2>, Info2...> e2) noexcept {
  using LE = add_to_at_t<0, N2, decltype(expand_error(e1))>;
  using RE = add_to_at_t<N1, N1, decltype(expand_error(e2))>;
  return concat_error_t<LE, RE>{};
}

// negation is defined under sized_true.h


/// Instantiation is required to print the type of the parameter pack in the error message on some compilers

template<typename Constraints, typename OptionalInfo>
struct because<Constraints, OptionalInfo> : Constraints {};

// nested fail with reason (optional)
template<typename Constraints1, std::size_t I, std::size_t N, typename... Reason,
         typename... Rest>
struct constraints_not_satisfied<
    Constraints1, at<I, N>, because<Reason...>,
    Rest... > : std::false_type {};

// fail with 2 disjunction
template<typename Constraints1, std::size_t I1, std::size_t N1,
         typename Constraints2, std::size_t I2, std::size_t N2,
         typename... Rest>
struct constraints_not_satisfied<
          Constraints1, at<I1, N1>,
    and_, Constraints2, at<I2, N2>,
    Rest... > : std::false_type {};

template<typename Constraints1, std::size_t I1, std::size_t N1, typename... Reason1,
         typename Constraints2, std::size_t I2, std::size_t N2,
         typename... Rest>
struct constraints_not_satisfied<
          Constraints1, at<I1, N1>, because<Reason1...>,
    and_, Constraints2, at<I2, N2>,
    Rest... > : std::false_type {};

template<typename Constraints1, std::size_t I1, std::size_t N1,
         typename Constraints2, std::size_t I2, std::size_t N2, typename... Reason2,
         typename... Rest>
struct constraints_not_satisfied<
          Constraints1, at<I1, N1>,
    and_, Constraints2, at<I2, N2>, because<Reason2...>,
    Rest... > : std::false_type {};

template<typename Constraints1, std::size_t I1, std::size_t N1, typename... Reason1,
         typename Constraints2, std::size_t I2, std::size_t N2, typename... Reason2,
         typename... Rest>
struct constraints_not_satisfied<
          Constraints1, at<I1, N1>, because<Reason1...>,
    and_, Constraints2, at<I2, N2>, because<Reason2...>,
    Rest... > : std::false_type {};

// fail with 3 disjunction
template<typename Constraints1, std::size_t I1, std::size_t N1,
         typename Constraints2, std::size_t I2, std::size_t N2,
         typename Constraints3, std::size_t I3, std::size_t N3,
         typename... Rest>
struct constraints_not_satisfied<
          Constraints1, at<I1, N1>,
    and_, Constraints2, at<I2, N2>,
    and_, Constraints3, at<I3, N3>,
    Rest... > : std::false_type {};

template<typename Constraints1, std::size_t I1, std::size_t N1,
         typename Constraints2, std::size_t I2, std::size_t N2,
         typename Constraints3, std::size_t I3, std::size_t N3, typename... Reason3,
         typename... Rest>
struct constraints_not_satisfied<
          Constraints1, at<I1, N1>,
    and_, Constraints2, at<I2, N2>,
    and_, Constraints3, at<I3, N3>, because<Reason3...>,
    Rest... > : std::false_type {};

template<typename Constraints1, std::size_t I1, std::size_t N1,
         typename Constraints2, std::size_t I2, std::size_t N2, typename... Reason2,
         typename Constraints3, std::size_t I3, std::size_t N3,
         typename... Rest>
struct constraints_not_satisfied<
          Constraints1, at<I1, N1>,
    and_, Constraints2, at<I2, N2>, because<Reason2...>,
    and_, Constraints3, at<I3, N3>,
    Rest... > : std::false_type {};

template<typename Constraints1, std::size_t I1, std::size_t N1,
         typename Constraints2, std::size_t I2, std::size_t N2, typename... Reason2,
         typename Constraints3, std::size_t I3, std::size_t N3, typename... Reason3,
         typename... Rest>
struct constraints_not_satisfied<
          Constraints1, at<I1, N1>,
    and_, Constraints2, at<I2, N2>, because<Reason2...>,
    and_, Constraints3, at<I3, N3>, because<Reason3...>,
    Rest... > : std::false_type {};

template<typename Constraints1, std::size_t I1, std::size_t N1, typename... Reason1,
         typename Constraints2, std::size_t I2, std::size_t N2,
         typename Constraints3, std::size_t I3, std::size_t N3,
         typename... Rest>
struct constraints_not_satisfied<
          Constraints1, at<I1, N1>, because<Reason1...>,
    and_, Constraints2, at<I2, N2>,
    and_, Constraints3, at<I3, N3>,
    Rest... > : std::false_type {};

template<typename Constraints1, std::size_t I1, std::size_t N1, typename... Reason1,
         typename Constraints2, std::size_t I2, std::size_t N2,
         typename Constraints3, std::size_t I3, std::size_t N3, typename... Reason3,
         typename... Rest>
struct constraints_not_satisfied<
          Constraints1, at<I1, N1>, because<Reason1...>,
    and_, Constraints2, at<I2, N2>,
    and_, Constraints3, at<I3, N3>, because<Reason3...>,
    Rest... > : std::false_type {};

template<typename Constraints1, std::size_t I1, std::size_t N1, typename... Reason1,
         typename Constraints2, std::size_t I2, std::size_t N2, typename... Reason2,
         typename Constraints3, std::size_t I3, std::size_t N3,
         typename... Rest>
struct constraints_not_satisfied<
          Constraints1, at<I1, N1>, because<Reason1...>,
    and_, Constraints2, at<I2, N2>, because<Reason2...>,
    and_, Constraints3, at<I3, N3>,
    Rest... > : std::false_type {};

template<typename Constraints1, std::size_t I1, std::size_t N1, typename... Reason1,
         typename Constraints2, std::size_t I2, std::size_t N2, typename... Reason2,
         typename Constraints3, std::size_t I3, std::size_t N3, typename... Reason3,
         typename... Rest>
struct constraints_not_satisfied<
          Constraints1, at<I1, N1>, because<Reason1...>,
    and_, Constraints2, at<I2, N2>, because<Reason2...>,
    and_, Constraints3, at<I3, N3>, because<Reason3...>,
    Rest... > : std::false_type {};

} // namespace concepts
} // namespace preview

#endif // PREVIEW_CONCEPTS_V2_DETAIL_CONSTRAINTS_NOT_SATISFIED_H_
