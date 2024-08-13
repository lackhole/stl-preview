//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_TYPE_TRAITS_SIMPLE_COMMON_REFERENCE_H_
#define PREVIEW_TYPE_TRAITS_SIMPLE_COMMON_REFERENCE_H_

#include <type_traits>

#include "preview/__type_traits/detail/test_ternary.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/no_traits.h"
#include "preview/__type_traits/typename_type_or.h"
#include "preview/__type_traits/type_identity.h"

namespace preview {
namespace detail {

// simple_common_reference_rref
template<typename T1, typename T2, typename C>
struct simple_common_reference_rref_2
    : std::conditional_t<
        conjunction< std::is_convertible<T1, C>, std::is_convertible<T2, C> >::value,
        type_identity<C>,
        no_traits
    > {};

template<
    typename T1,
    typename T2,
    typename CREF,
    bool = /* true */ has_typename_type<CREF>::value>
struct simple_common_reference_rref
    : simple_common_reference_rref_2<
        T1,
        T2,
        std::remove_reference_t<typename CREF::type>&&
    > {};

template<typename T1, typename T2, typename CWrap>
struct simple_common_reference_rref<T1, T2, CWrap, false> {};


// simple_common_reference_mixed_ref

template<
    typename B,
    typename DREF,
    bool = /* true */ has_typename_type<DREF>::value>
struct simple_common_reference_mixed_ref
    : std::conditional_t<
        std::is_convertible< B, typename DREF::type >::value,
        type_identity<typename DREF::type>,
        no_traits
    > {};

template<typename B, typename DREF>
struct simple_common_reference_mixed_ref<B, DREF, false> {};

} // namespace detail

template<typename T1, typename T2>
struct simple_common_reference {}; // no simple common reference type

template<typename T1, typename T2>
struct simple_common_reference<T1&, T2&>
    : typename_type_or<detail::test_ternary_invoke<copy_cv_t<T1, T2>&, copy_cv_t<T2, T1>&>, no_traits, true> {};

template<typename T1, typename T2>
struct simple_common_reference<T1&&, T2&&>
    : detail::simple_common_reference_rref<T1, T2, /*CREF=*/simple_common_reference<T1&, T2&>> {};

template<typename A, typename B>
struct simple_common_reference<A&, B&&>
    : detail::simple_common_reference_mixed_ref<B&&, /*DREF=*/simple_common_reference<A&, B const&>>{};

template<typename B, typename A>
struct simple_common_reference<B&&, A&> : simple_common_reference<A&, B&&> {};

template<typename T1, typename T2>
using simple_common_reference_t = typename simple_common_reference<T1, T2>::type;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_SIMPLE_COMMON_REFERENCE_H_
