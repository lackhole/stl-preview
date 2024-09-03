//
// Created by yonggyulee on 2024. 9. 4.
//

#ifndef PREVIEW_CONCEPTS_V2_DETAIL_CONCEPT_BASE_H_
#define PREVIEW_CONCEPTS_V2_DETAIL_CONCEPT_BASE_H_

#include "preview/__core/inline_variable.h"
#include "preview/__concepts_v2/detail/constraints_not_satisfied.h"
#include "preview/__concepts_v2/detail/sized_true.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/disjunction.h"

namespace preview {
namespace concepts {

template<bool B> struct valid_tag {};
template<> struct valid_tag<true> { using valid = bool; };

template<typename Derived, typename Base>
struct concept_base : Base, valid_tag<Base::value> {
 private:
  template<typename C2, typename B2>
  static constexpr sized_true<2> conj_c_c(concept_base, concept_base<C2, B2>, std::true_type, std::true_type)
      { return {}; }
  template<typename C2, typename B2>
  static constexpr auto conj_c_c(concept_base, concept_base<C2, B2>, std::true_type, std::false_type)
      { return expand_error(constraints_not_satisfied<C2, at<1, 2>>{}); }
  template<typename C2, typename B2, bool B>
  static constexpr auto conj_c_c(concept_base, concept_base<C2, B2>, std::false_type, std::integral_constant<bool, B>)
      { expand_error(constraints_not_satisfied<Derived, at<0, 2>>{}); }

  template<std::size_t N>
  static constexpr sized_true<N + 1> conj_c_t(concept_base, sized_true<N>, std::true_type)
      { return {}; }
  template<std::size_t N>
  static constexpr auto conj_c_t(concept_base, sized_true<N>, std::false_type)
      { return expand_error(constraints_not_satisfied<Derived, at<0, N + 1>>{}); }
  template<std::size_t N>
  static constexpr sized_true<N + 1> conj_t_c(sized_true<N>, concept_base, std::true_type)
      { return {}; }
  template<std::size_t N>
  static constexpr auto conj_t_c(sized_true<N>, concept_base, std::false_type)
      { return expand_error(constraints_not_satisfied<Derived, at<N, N + 1>>{}); }

  template<typename Error, std::size_t I, std::size_t N, typename... ErrorInfo>
  static constexpr auto conj_c_f(concept_base, constraints_not_satisfied<Error, at<I, N>, ErrorInfo...> e, std::true_type)
      { return add_to_at_t<1, 1, decltype(e)>{}; }
  template<typename Error, std::size_t I, std::size_t N, typename... ErrorInfo>
  static constexpr auto conj_c_f(concept_base, constraints_not_satisfied<Error, at<I, N>, ErrorInfo...> e, std::false_type)
      { return expand_error(constraints_not_satisfied<Derived, at<0, N + 1>>{}); }


  template<typename C2, typename B2>
  static constexpr auto disj_c_c(concept_base, concept_base<C2, B2>, std::true_type)
      { return sized_true<2>{}; }
  template<typename C2, typename B2>
  static constexpr auto disj_c_c(concept_base, concept_base<C2, B2>, std::false_type) {
    using E1 = decltype(expand_error(constraints_not_satisfied<Derived, at<0, 2>>{}));
    using E2 = decltype(expand_error(constraints_not_satisfied<C2, at<1, 2>>{}));
    return concat_error_t<E1, E2>{};
  }

  template<typename Error, std::size_t I, std::size_t N, typename... ErrorInfo>
  static constexpr auto disj_c_f(concept_base, constraints_not_satisfied<Error, at<I, N>, ErrorInfo...>, std::true_type)
      { return sized_true<N + 1>{}; }
  template<typename Error, std::size_t I, std::size_t N, typename... ErrorInfo>
  static constexpr auto disj_c_f(concept_base, constraints_not_satisfied<Error, at<I, N>, ErrorInfo...> e, std::false_type) {
    using E1 = decltype(expand_error(constraints_not_satisfied<Derived, at<0, N + 1>>{}));
    using E2 = add_to_at_t<1, 1, decltype(e)>;
    return concat_error_t<E1, E2>{};
  }

  template<typename Error, std::size_t I, std::size_t N, typename... ErrorInfo>
  static constexpr auto disj_f_c(constraints_not_satisfied<Error, at<I, N>, ErrorInfo...>, concept_base, std::true_type)
      { return sized_true<N + 1> {}; }
  template<typename Error, std::size_t I, std::size_t N, typename... ErrorInfo>
  static constexpr auto disj_f_c(constraints_not_satisfied<Error, at<I, N>, ErrorInfo...> e, concept_base, std::false_type) {
    using E1 = add_to_at_t<0, 1, decltype(e)>;
    using E2 = decltype(expand_error(constraints_not_satisfied<Derived, at<N, N + 1>>{}));
    return concat_error_t<E1, E2>{};
  }

 public:
  using base = Base;

  /// conjunctions

  // concept && concept
  template<typename C2, typename B2>
  friend constexpr auto operator&&(concept_base x, concept_base<C2, B2> y) noexcept {
    return conj_c_c(x, y, bool_constant<Base::value>{}, bool_constant<B2::value>{});
  }

  // concept && true
  template<std::size_t N>
  friend constexpr auto operator&&(concept_base, sized_true<N>) noexcept {
    return conj_c_t(concept_base{}, sized_true<N>{}, bool_constant<Base::value>{});
  }

  // true && concept
  template<std::size_t N>
  friend constexpr auto operator&&(sized_true<N>, concept_base) noexcept {
    return conj_t_c(sized_true<N>{}, concept_base{}, bool_constant<Base::value>{});
  }

  // concept && false
  template<typename Error, std::size_t I, std::size_t N, typename... ErrorInfo>
  friend constexpr auto operator&&(concept_base, constraints_not_satisfied<Error, at<I, N>, ErrorInfo...> e) noexcept {
    return conj_c_f(concept_base{}, e, bool_constant<Base::value>{});
  }

  // false && concept
  template<typename Error, std::size_t I, std::size_t N, typename... ErrorInfo>
  friend constexpr auto operator&&(constraints_not_satisfied<Error, at<I, N>, ErrorInfo...> e, concept_base) noexcept {
    return add_to_at_t<0, 1, decltype(e)>{};
  }

  // concept && bool_constant
  template<typename Other, std::enable_if_t<preview::conjunction<
      preview::disjunction<
          std::is_base_of<std::true_type, Other>,
          std::is_base_of<std::false_type, Other>
      >,
      preview::negation<is_concept<Other>>
  >::value, int> = 0>
  friend constexpr auto operator&&(concept_base, Other) noexcept {
    return Derived{} && concept_base<Other, bool_constant<Other::value>>{};
  }

  // bool_constant && concept
  template<typename Other, std::enable_if_t<preview::conjunction<
      preview::disjunction<
          std::is_base_of<std::integral_constant<bool, true>, Other>,
          std::is_base_of<std::integral_constant<bool, false>, Other>
      >,
      preview::negation<is_concept<Other>>
  >::value, int> = 0>
  friend constexpr auto operator&&(Other, concept_base) noexcept {
    return concept_base<Other, bool_constant<Other::value>>{} && Derived{};
  }

  /// disjunctions

  // concept || concept
  template<typename C2, typename B2>
  friend constexpr auto operator||(concept_base x, concept_base<C2, B2> y) noexcept {
    return disj_c_c(x, y, disjunction<Base, B2>{});
  }

  // concept || true
  template<std::size_t N>
  friend constexpr auto operator||(concept_base, sized_true<N>) noexcept {
    return sized_true<N + 1>{};
  }

  // true || concept
  template<std::size_t N>
  friend constexpr auto operator||(sized_true<N>, concept_base) noexcept {
    return sized_true<N + 1>{};
  }

  // concept || false
  template<typename Error, std::size_t I, std::size_t N, typename... ErrorInfo>
  friend constexpr auto operator||(concept_base x, constraints_not_satisfied<Error, at<I, N>, ErrorInfo...> y) noexcept {
    return disj_c_f(x, y, bool_constant<Base::value>{});
  }

  // false || concept
  template<typename Error, std::size_t I, std::size_t N, typename... ErrorInfo>
  friend constexpr auto operator||(constraints_not_satisfied<Error, at<I, N>, ErrorInfo...> x, concept_base y) noexcept {
    return disj_f_c(x, y, bool_constant<Base::value>{});
  }

  // concept || bool_constant
  template<typename Other, std::enable_if_t<preview::conjunction<
      preview::disjunction<
          std::is_base_of<std::integral_constant<bool, true>, Other>,
          std::is_base_of<std::integral_constant<bool, false>, Other>
      >,
      preview::negation<is_concept<Other>>
  >::value, int> = 0>
  friend constexpr auto operator||(concept_base, Other) noexcept {
    return Derived{} || concept_base<Other, Other>{};
  }

  // bool_constant || concept
  template<typename Other, std::enable_if_t<preview::conjunction<
      preview::disjunction<
          std::is_base_of<std::integral_constant<bool, true>, Other>,
          std::is_base_of<std::integral_constant<bool, false>, Other>
      >,
      preview::negation<is_concept<Other>>
  >::value, int> = 0>
  friend constexpr auto operator||(Other, concept_base) noexcept {
    return concept_base<Other, bool_constant<Other::value>>{} || Derived{};
  }


  /// negation
  friend constexpr negation<Derived> operator!(concept_base) noexcept { return {}; }
};

} // namespace concepts
} // namespace preview

#define PREVIEW_DEFINE_CONCEPT_UNDER(ns, tmp, name, inst, ...) \
namespace ns {                                                 \
tmp                                                            \
struct name                                                    \
    : preview::concepts::concept_base<inst, decltype(__VA_ARGS__)> {};   \
}                                                              \
                                                               \
tmp                                                            \
PREVIEW_INLINE_VARIABLE constexpr ns::inst name                \


#endif // PREVIEW_CONCEPTS_V2_DETAIL_CONCEPT_BASE_H_
