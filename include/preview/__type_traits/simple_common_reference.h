//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_TYPE_TRAITS_SIMPLE_COMMON_REFERENCE_H_
#define PREVIEW_TYPE_TRAITS_SIMPLE_COMMON_REFERENCE_H_

#include <type_traits>

#include "preview/__type_traits/detail/test_ternary.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/type_identity.h"

namespace preview {
namespace impl {

struct no_simple_common_reference {};


// simple_common_reference_lref

template<
    typename CV12XR,
    typename CV12YR,
    bool = has_typename_type<detail::test_ternary<CV12XR, CV12YR>>::value>
struct simple_common_reference_lref;

template<typename T1, typename T2>
struct simple_common_reference_lref<T1, T2, true>
    : std::conditional_t<
        std::is_reference< detail::test_ternary_t<T1, T2> >::value,
        type_identity<detail::test_ternary_t<T1, T2>>,
        no_simple_common_reference
      > {};

template<typename T1, typename T2>
struct simple_common_reference_lref<T1, T2, false> {};


// simple_common_reference_rref

template<typename T1, typename T2, typename C>
struct simple_common_reference_rref_2
    : std::conditional_t<
        conjunction< std::is_convertible<T1, C>, std::is_convertible<T2, C> >::value,
        type_identity<C>,
        no_simple_common_reference> {};

template<
    typename T1,
    typename T2,
    typename CWrap,
    bool = has_typename_type<CWrap>::value>
struct simple_common_reference_rref;

template<typename T1, typename T2, typename CWrap>
struct simple_common_reference_rref<T1, T2, CWrap, true>
    : simple_common_reference_rref_2<
        T1,
        T2,
        std::add_rvalue_reference_t< std::remove_reference_t<typename CWrap::type> >
      > {};

template<typename T1, typename T2, typename CWrap>
struct simple_common_reference_rref<T1, T2, CWrap, false> {};


// simple_common_reference_mixed_ref

template<
    typename B,
    typename DWrap,
    bool = has_typename_type<DWrap>::value>
struct simple_common_reference_mixed_ref;

template<typename B, typename DWrap>
struct simple_common_reference_mixed_ref<B, DWrap, true>
    : std::conditional_t<
        std::is_convertible< B, typename DWrap::type >::value,
        type_identity<typename DWrap::type>,
        no_simple_common_reference
      >{};

template<typename B, typename DWrap>
struct simple_common_reference_mixed_ref<B, DWrap, false> {};

} // namespace impl

template<typename T1, typename T2>
struct simple_common_reference {}; // no simple common reference type

template<typename T1, typename T2>
struct simple_common_reference<T1&, T2&>
    : impl::simple_common_reference_lref<copy_cv_t<T2&, T1&>, copy_cv_t<T1&, T2&>> {};

template<typename T1, typename T2>
struct simple_common_reference<T1&&, T2&&>
    : impl::simple_common_reference_rref<T1, T2, /*CWrap=*/simple_common_reference<T1&, T2&>> {};

template<typename A, typename B>
struct simple_common_reference<A&, B&&>
    : impl::simple_common_reference_mixed_ref<B&&, /*DWrap=*/simple_common_reference<A&, B const&>>{};

template<typename B, typename A>
struct simple_common_reference<B&&, A&> : simple_common_reference<A&, B&&> {};

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_SIMPLE_COMMON_REFERENCE_H_
