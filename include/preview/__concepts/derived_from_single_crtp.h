//
// Created by yonggyulee on 2024/01/01.
//

#ifndef PREVIEW_TYPE_TRAITS_IS_DERIVED_FROM_SINGLE_CRTP_H_
#define PREVIEW_TYPE_TRAITS_IS_DERIVED_FROM_SINGLE_CRTP_H_

#include "preview/__concepts/same_as.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename T, typename = void>

struct has_typename_preview_derived : std::false_type {};
template<typename T>
struct has_typename_preview_derived<T, void_t<typename T::_$preview_derived>> : std::true_type {};

template<template<typename> class Base>
struct derived_from_single_crtp_tester {
  template<typename T, typename U>
  auto operator()(const T&, const Base<U>&) const -> same_as<T, Base<U>>;
};

template<
    typename Derived,
    template<typename> class Base,
    bool = is_referencable<Derived>::value /* true */
>
struct derived_from_single_crtp_impl
#if defined(_MSC_VER) && _MSC_VER < 1930 // Ambigious casting is allowed until Visutal Studio 2022
    : has_typename_preview_derived<Derived> {};
#else
    : is_invocable_r<
          std::false_type, derived_from_single_crtp_tester<Base>, Derived&, Derived&>{};
#endif

template<typename Derived, template<typename> class Base>
struct derived_from_single_crtp_impl<Derived, Base, false> : std::false_type {};

} // namespace detail

template<typename Derived, template<typename> class Base>
struct derived_from_single_crtp : detail::derived_from_single_crtp_impl<Derived, Base> {};

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_IS_DERIVED_FROM_SINGLE_CRTP_H_
