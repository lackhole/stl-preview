//
// Created by yonggyulee on 2024/01/06.
//

#ifndef PREVIEW_RANGES_TO_H_
#define PREVIEW_RANGES_TO_H_

#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

#include "preview/__algorithm/ranges/for_each.h"
#include "preview/__concepts/constructible_from.h"
#include "preview/__concepts/derived_from.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/iter_key_t.h"
#include "preview/__iterator/iter_mapped_t.h"
#include "preview/__ranges/detail/container_appender.h"
#include "preview/__ranges/detail/reservable_container.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/from_range.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/range_adaptor_closure.h"
#include "preview/__ranges/range_key_t.h"
#include "preview/__ranges/range_mapped_t.h"
#include "preview/__ranges/range_size_t.h"
#include "preview/__ranges/ref_view.h"
#include "preview/__ranges/sentinel_t.h"
#include "preview/__ranges/size.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/views/transform.h"
#include "preview/__tuple/make_from_tuple.h"
#include "preview/__tuple/tuple_like.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/detail/tag.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_deductible.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/template_arity.h"
#include "preview/__type_traits/void_t.h"
#include "preview/__utility/in_place.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename C, typename R, std::enable_if_t<conjunction<sized_range<R>, reservable_container<C>>::value, int> = 0>
constexpr void try_reserve(C& c, R&& r) {
  c.reserve(static_cast<range_size_t<C>>(ranges::size(r)));
}
template<typename C, typename R, std::enable_if_t<conjunction<sized_range<R>, reservable_container<C>>::value == false, int> = 0>
constexpr void try_reserve(C&, R&&) {}

template<typename R, typename T, bool = input_range<R>::value /* true */>
struct container_compatible_range : convertible_to<range_reference_t<R>, T> {};
template<typename R, typename T>
struct container_compatible_range<R, T, false> : std::false_type {};

using preview::detail::tag_1;
using preview::detail::tag_2;
using preview::detail::tag_3;
using preview::detail::tag_4;
using preview::detail::tag_5;

template<bool /* false */, typename C, typename R, typename... Args>
struct ranges_to_iterator_sentinel_constructible_impl
    : std::false_type {};

template<typename C, typename R, typename... Args>
struct ranges_to_iterator_sentinel_constructible_impl<true, C, R, Args...>
    : conjunction<
        derived_from<typename iterator_traits<iterator_t<R>>::iterator_category, input_iterator_tag>,
        constructible_from<C, iterator_t<R>, sentinel_t<R>, Args...>
    > {};

template<bool /* common_range = false */, typename C, typename R, typename... Args>
struct ranges_to_iterator_sentinel_constructible
    : std::false_type {};

template<typename C, typename R, typename... Args>
struct ranges_to_iterator_sentinel_constructible<true, C, R, Args...>
    : ranges_to_iterator_sentinel_constructible_impl<
        preview::detail::has_typename_iterator_category< iterator_traits<iterator_t<R>> >::value,
        C, R, Args...
    > {};

#if PREVIEW_CXX_VERSION >= 23
template<typename From, typename To>
struct cxx23_convertible_to : convertible_to<From, To> {};
#else
template<typename From, typename To, bool = conjunction<tuple_like<To>, tuple_like<From>>::value /* false */>
struct cxx23_convertible_to : convertible_to<From, To> {};
template<typename From, typename To>
struct cxx23_convertible_to<From, To, true>
    : conjunction<preview::detail::is_constructible_from_tuple<To, From>> {};
#endif

template<typename C, typename R, typename... Args>
using ranges_to_unnested_tag = preview::detail::conditional_tag<
    constructible_from<C, R, Args...>,
    constructible_from<C, from_range_t, R, Args...>,
    ranges_to_iterator_sentinel_constructible<common_range<R>::value, C, R, Args...>,
    conjunction<
        constructible_from<C, Args...>,
        container_appendable<C, range_reference_t<R>>
    >
>;

template<typename C, typename R, typename... Args>
struct ranges_to_unnested_callable
    : conjunction<
        disjunction<
            negation<input_range<C>>,
            cxx23_convertible_to<range_reference_t<R>, range_value_t<C>>
        >,
        bool_constant<(detail::ranges_to_unnested_tag<C, R, Args...>::value != 0)>
    > {};

template<typename C, typename R, typename... Args>
struct ranges_to_nested_callable : input_range<range_reference_t<C>> {};

template<typename C, typename R, typename... Args>
constexpr C ranges_to_unnested(tag_1, R&& r, Args&&... args) {
  return C(std::forward<R>(r), std::forward<Args>(args)...);
}

template<typename C, typename R, typename... Args>
constexpr C ranges_to_unnested(tag_2, R&& r, Args&&... args) {
  return C(from_range, std::forward<R>(r), std::forward<Args>(args)...);
}

// after C++23 (or clang tuple-to-pair-extension falls here)
template<typename C, typename R, typename...Args>
constexpr C ranges_to_iterator_sentinel(std::false_type, R&& r, Args&&... args) {
  return C(ranges::begin(r), ranges::end(r), std::forward<Args>(args)...);
}

// before C++23
template<typename C, typename R, typename...Args>
constexpr C ranges_to_iterator_sentinel(std::true_type, R&& r, Args&&... args) {
  return ranges_to_iterator_sentinel<C>(
      std::false_type{},
      views::transform(std::forward<R>(r), [](auto&& t) {
        return preview::make_from_tuple<range_value_t<C>>(std::forward<decltype(t)>(t));
      }),
      std::forward<Args>(args)...
  );
}

template<typename C, typename R, typename... Args>
constexpr C ranges_to_unnested(tag_3, R&& r, Args&&... args) {
  using tag_t = conjunction<
      cxx23_convertible_to<range_reference_t<R>, range_value_t<C>>,
      negation< convertible_to<range_reference_t<R>, range_value_t<C>> >
  >;
  return ranges_to_iterator_sentinel<C>(tag_t{}, std::forward<R>(r), std::forward<Args>(args)...);
}

template<typename C, typename R, typename... Args>
constexpr C ranges_to_unnested(tag_4, R&& r, Args&&... args) {
  C c(std::forward<Args>(args)...);
  preview::ranges::detail::try_reserve(c, r);
  ranges::for_each(r, container_appender(c));
  return c;
}

template<typename C, typename R, typename... Args>
constexpr C ranges_to(std::true_type /* unnested */, R&& r, Args&&... args) {
  using tag_t = ranges_to_unnested_tag<C, R, Args...>;
  return preview::ranges::detail::ranges_to_unnested<C>(tag_t{}, std::forward<R>(r), std::forward<Args>(args)...);
}

template<typename C, typename R, typename... Args>
constexpr C ranges_to(std::false_type/* unnested */, R&& r, Args&&... args);

struct as_pair_fn {
  template<typename Tuple, std::enable_if_t<pair_like<Tuple>::value, int> = 0>
  constexpr auto operator()(Tuple&& t) const {
    return std::make_pair(std::get<0>(std::forward<Tuple>(t)), std::get<1>(std::forward<Tuple>(t)));
  }

  template<typename T, std::enable_if_t<pair_like<T>::value == false, int> = 0>
  constexpr decltype(auto) operator()(T&& t) const {
    return std::forward<T>(t);
  }
};

template<typename R>
using as_pair_view_t = decltype(views::transform(std::declval<R>(), as_pair_fn{}));

template<typename R>
struct ranges_to_input_iterator {
  using iterator_category = input_iterator_tag;
  using value_type        = range_value_t<R>;
  using difference_type   = std::ptrdiff_t;
  using pointer           = std::add_pointer_t<range_reference_t<R>>;
  using reference         = range_reference_t<R>;

  reference operator*() const;
  pointer operator->() const;
  ranges_to_input_iterator& operator++();
  ranges_to_input_iterator operator++(int);
  bool operator==(const ranges_to_input_iterator&) const;
};

struct not_deductible : std::false_type {};

#if PREVIEW_CXX_VERSION >= 17
template<template<typename...> class C, typename R, typename... Args>
struct deduce_direct : std::true_type {
  using expr = decltype(C(std::declval<R>(), std::declval<Args>()...));
};
template<template<typename...> class C, typename R, typename... Args>
struct deduce_from_range : std::true_type {
  using expr = decltype(C(from_range, std::declval<R>(), std::declval<Args>()...));
};
template<template<typename...> class C, typename R, typename... Args>
struct deduce_iterator : std::true_type {
  using expr = decltype(C(std::declval<ranges_to_input_iterator<R>>(), std::declval<ranges_to_input_iterator<R>>(), std::declval<Args>()...));
};

template<template<typename...> class C, typename R, typename... Args>
struct deduce_expr_t_default : conditional_t<
    is_deductible<C, R, Args...>,                                                           deduce_direct    <C, R, Args...>,
    is_deductible<C, from_range_t, R, Args...>,                                             deduce_from_range<C, R, Args...>,
    is_deductible<C, ranges_to_input_iterator<R>, ranges_to_input_iterator<R>, Args&&...>,  deduce_iterator  <C, R, Args...>,
    not_deductible
> {};

template<template<typename...> class C, typename R, typename... Args>
struct deduce_iter_key_mapped_dumb : std::true_type {
  using expr = C<iter_key_t<iterator_t<R>>, iter_mapped_t<iterator_t<R>>, Args...>;
};

template<template<typename...> class C, typename R, typename... Args>
struct deduce_expr_t_key_value : conditional_t<
    is_deductible<C, as_pair_view_t<R>, Args...>,                                           deduce_direct<C, as_pair_view_t<R>, Args...>,
    is_deductible<C, from_range_t, as_pair_view_t<R>, Args...>,                             deduce_from_range<C, as_pair_view_t<R>, Args...>,
    is_deductible<C, ranges_to_input_iterator<as_pair_view_t<R>>,
                     ranges_to_input_iterator<as_pair_view_t<R>>, Args...>,                 deduce_iterator<C, as_pair_view_t<R>, Args...>,
    // Defense for Android NDK < 21 vvvvvvvvvv
    conjunction<
        has_typename_type<iter_key<iterator_t<R>>>,
        has_typename_type<iter_mapped<iterator_t<R>>>,
        std::is_constructible<
            C<iter_key_t<iterator_t<R>>, iter_mapped_t<iterator_t<R>>, Args...>,
            ranges_to_input_iterator<R>,
            ranges_to_input_iterator<R>,
            Args...
        >
    >,                                                                                      deduce_iter_key_mapped_dumb<C, R, Args...>,
    // Defense for Android NDK < 21 ^^^^^^^^^^
    not_deductible
> {};

template<template<typename...> class C, typename R, typename... Args>
struct deduce_expr_t : conditional_t<
    is_deductible<C, R, Args...>,                                                           deduce_direct    <C, R, Args...>,
    is_deductible<C, from_range_t, R, Args...>,                                             deduce_from_range<C, R, Args...>,
    is_deductible<C, ranges_to_input_iterator<R>, ranges_to_input_iterator<R>, Args&&...>,  deduce_iterator  <C, R, Args...>,
    // Defense for C++23 vvvvvvvvvv
    conjunction<bool_constant<mandatory_template_arity<C>::value == 2>,
                pair_like<range_value_t<R>>>,                                               deduce_expr_t_key_value<C, R, Args...>,
    // Defense for C++23 ^^^^^^^^^^
    not_deductible
> {};
#else // PREVIEW_CXX_VERSION >= 17

template<template<typename...> class C, typename R, typename... Args>
struct deduce_direct : std::true_type {
  using expr = C<range_value_t<R>, Args...>;
};
template<template<typename...> class C, typename R, typename... Args>
struct deduce_from_range : std::true_type {
  using expr = C<range_value_t<R>, Args...>;
};
template<template<typename...> class C, typename R, typename... Args>
struct deduce_iterator : std::true_type {
  using expr = C<range_value_t<R>, Args...>;
};
template<template<typename...> class C, typename R, typename... Args>
struct deduce_key_mapped : std::true_type {
  using expr = C<range_key_t<R>, range_mapped_t<R>, Args...>;
};

template<typename R>
struct is_key_value_range
    : conjunction<
        input_range<R>,
        has_typename_type<range_key<R>>,
        has_typename_type<range_mapped<R>>
      > {};

template<template<typename...> class C>
struct maybe_key_value_container
    : conjunction<
        bool_constant<(mandatory_template_arity<C>::value == 2)>
      > {};

template<template<typename...> class C, typename R, typename... Args>
struct deduce_expr_t_key_value : conditional_t<
    std::is_constructible<C<range_key_t<R>, range_mapped_t<R>, Args...>, R, Args...>,
        deduce_key_mapped<C, R, Args...>,
    std::is_constructible<C<range_key_t<R>, range_mapped_t<R>, Args...>, from_range_t, R&&, Args&&...>,
        deduce_key_mapped<C, R, Args...>,
    std::is_constructible< C<range_key_t<R>, range_mapped_t<R>, Args...>, ranges_to_input_iterator<R>&&, ranges_to_input_iterator<R>&&>,
        deduce_key_mapped<C, R, Args...>,
    not_deductible
> {};

template<template<typename...> class C, typename R, typename... Args>
struct deduce_expr_t_default : conditional_t<
    std::is_constructible<C<range_value_t<R>, Args...>, R&&, Args&&...>,
        deduce_direct<C, R, Args...>,
    std::is_constructible<C<range_value_t<R>, Args...>, from_range_t, R&&, Args&&... >,
        deduce_from_range<C, R, Args...>,
    std::is_constructible<C<range_value_t<R>, Args...>, ranges_to_input_iterator<R>&&, ranges_to_input_iterator<R>&& >,
        deduce_iterator<C, R, Args...>,
    not_deductible
> {};

template<template<typename...> class C, typename R, typename... Args>
struct deduce_expr_t : conditional_t<
    bool_constant<(mandatory_template_arity<C>::value == 1)>,         deduce_expr_t_default<C, R, Args...>,
    conjunction<maybe_key_value_container<C>, is_key_value_range<R>>, deduce_expr_t_key_value<C, R, Args...>,
    not_deductible
> {};

#endif

template<template<typename...> class C, typename R, typename... Args>
using deduce_expr = typename deduce_expr_t<C, R, Args...>::expr;

} // namespace detail

template<typename C, typename R, typename... Args, std::enable_if_t<conjunction<
    input_range<R>,
    negation< view<C> >,
    disjunction<
        detail::ranges_to_unnested_callable<C, R, Args...>,
        detail::ranges_to_nested_callable<C, R, Args...>
    >
>::value, int> = 0>
constexpr C to(R&& r, Args&&... args) {
  using unnested = detail::ranges_to_unnested_callable<C, R, Args...>;
  return preview::ranges::detail::ranges_to<C>(unnested{}, std::forward<R>(r), std::forward<Args>(args)...);
}

template<template<typename...> class C, typename R, typename... Args, std::enable_if_t<input_range<R>::value, int> = 0>
constexpr detail::deduce_expr<C, R, Args...> to(R&& r, Args&&... args) {
  return preview::ranges::to<detail::deduce_expr<C, R, Args...>>(std::forward<R>(r), std::forward<Args>(args)...);
}

namespace detail {

template<typename C, typename R, typename... Args>
constexpr C ranges_to(std::false_type/* unnested */, R&& r, Args&&... args) {
  return preview::ranges::to<C>(
      ranges::ref_view<std::remove_reference_t<R>>(r) | views::transform([](auto&& elem) {
        return preview::ranges::to<range_value_t<C>>(std::forward<decltype(elem)>(elem));
      }),
      std::forward<Args>(args)...
  );
}

template<typename C, typename... Args>
class to_adaptor_closure : public range_adaptor_closure<to_adaptor_closure<C, Args...>> {
  template<typename R, typename... CVArgs>
  using callable = conjunction<
      input_range<R>,
      disjunction<
          detail::ranges_to_unnested_callable<C, R, CVArgs...>,
          detail::ranges_to_nested_callable<C, R, CVArgs...>
      >
  >;

 public:
  template<typename... T>
  constexpr explicit to_adaptor_closure(in_place_t, T&&... arg)
      : tup_(std::forward<T>(arg)...) {}

  template<typename R, std::enable_if_t<callable<R, Args&...>::value, int> = 0>
  constexpr C operator()(R&& r) & {
    return call(std::forward<R>(r), tup_, std::index_sequence_for<Args...>{});
  }

  template<typename R, std::enable_if_t<callable<R, const Args&...>::value, int> = 0>
  constexpr C operator()(R&& r) const& {
    return call(std::forward<R>(r), tup_, std::index_sequence_for<Args...>{});
  }

  template<typename R, std::enable_if_t<callable<R, Args...>::value, int> = 0>
  constexpr C operator()(R&& r) && {
    return call(std::forward<R>(r), std::move(tup_), std::index_sequence_for<Args...>{});
  }

  template<typename R, std::enable_if_t<callable<R, const Args...>::value, int> = 0>
  constexpr C operator()(R&& r) const && {
    return call(std::forward<R>(r), std::move(tup_), std::index_sequence_for<Args...>{});
  }

 private:
  template<typename R, typename Tuple, std::size_t...I>
  static constexpr C call(R&& r, Tuple&& t, std::index_sequence<I...>) {
    return preview::ranges::to<C>(std::forward<R>(r), std::get<I>(std::forward<Tuple>(t))...);
  }

  std::tuple<Args...> tup_;
};

template<template<typename...> class>
struct template_type_holder;

template<template<typename...> class C, typename... Args>
class to_adaptor_closure<template_type_holder<C>, Args...>
    : public range_adaptor_closure<to_adaptor_closure<template_type_holder<C>, Args...>>
{
 public:
  template<typename... T>
  constexpr explicit to_adaptor_closure(in_place_t, T&&... arg)
      : tup_(std::forward<T>(arg)...) {}

  template<typename R, std::enable_if_t<input_range<R>::value, int> = 0>
  constexpr detail::deduce_expr<C, R, Args&...> operator()(R&& r) & {
    return call(std::forward<R>(r), tup_, std::index_sequence_for<Args...>{});
  }

  template<typename R, std::enable_if_t<input_range<R>::value, int> = 0>
  constexpr detail::deduce_expr<C, R, const Args&...> operator()(R&& r) const& {
    return call(std::forward<R>(r), tup_, std::index_sequence_for<Args...>{});
  }

  template<typename R, std::enable_if_t<input_range<R>::value, int> = 0>
  constexpr detail::deduce_expr<C, R, Args...> operator()(R&& r) && {
    return call(std::forward<R>(r), std::move(tup_), std::index_sequence_for<Args...>{});
  }

  template<typename R, std::enable_if_t<input_range<R>::value, int> = 0>
  constexpr detail::deduce_expr<C, R, const Args...> operator()(R&& r) const && {
    return call(std::forward<R>(r), std::move(tup_), std::index_sequence_for<Args...>{});
  }

 private:
  template<typename R, typename Tuple, std::size_t...I>
  static constexpr auto call(R&& r, Tuple&& t, std::index_sequence<I...>) {
    return preview::ranges::to<C>(std::forward<R>(r), std::get<I>(std::forward<Tuple>(t))...);
  }

  std::tuple<Args...> tup_;
};

} // namespace detail

template<typename C, typename... Args, std::enable_if_t<negation<view<C>>::value, int> = 0>
constexpr auto to(Args&&... args) {
  return detail::to_adaptor_closure<C, std::decay_t<Args>...>(in_place, std::forward<Args>(args)...);
}

template<template<typename...> class C, typename... Args>
constexpr auto to(Args&&... args) {
  return detail::to_adaptor_closure<detail::template_type_holder<C>, std::decay_t<Args>...>(in_place, std::forward<Args>(args)...);
}

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_TO_H_
