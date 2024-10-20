//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_TYPE_TRAITS_COMMON_REFERENCE_H_
#define PREVIEW_TYPE_TRAITS_COMMON_REFERENCE_H_

#include <functional>
#include <type_traits>
#include <utility>

#include "preview/__concepts/convertible_to.h"
#include "preview/__type_traits/basic_common_reference.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/common_type.h"
#include "preview/__type_traits/copy_cvref.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/simple_common_reference.h"
#include "preview/__type_traits/detail/test_ternary.h"

namespace preview {

template<typename...>
struct common_reference;

namespace detail {

template<typename T>
struct basic_common_reference_qual_gen {
  template<typename U>
  using qual = copy_cvref_t<T, U>;
};

// https://eel.is/c++draft/meta.trans.other#6.3.1
// Let `R` be `COMMON-REF(T1, T2)`. If `T1` and `T2` are reference types, `R` is well-formed, and
// `is_convertible_v<add_pointer_t<T1>, add_pointer_t<R>> && is_convertible_v<add_pointer_t<T2>, add_pointer_t<R>>`
// is `true`, then the member typedef `type` denotes `R`.
template<typename T1, typename T2,
         bool = /* false */ conjunction<
             std::is_reference<T1>,
             std::is_reference<T2>,
             has_typename_type<simple_common_reference<T1, T2>>
         >::value>
struct common_reference_tag_1 : std::false_type {};

template<typename T1, typename T2>
struct common_reference_tag_1<T1, T2, true>
    : conjunction<
        std::is_convertible<std::add_pointer_t<T1>, std::add_pointer_t<simple_common_reference_t<T1, T2>>>,
        std::is_convertible<std::add_pointer_t<T2>, std::add_pointer_t<simple_common_reference_t<T1, T2>>>
    > {};

// https://eel.is/c++draft/meta.trans.other#6.3.2
// Otherwise, if `basic_common_reference<remove_cvref_t<T1>, remove_cvref_t<T2>, XREF(T1), XREF(T2)>::type` is
// well-formed, then the member typedef `type` denotes that type.
template<typename T1, typename T2>
struct common_reference_tag_2
    : has_typename_type<
        basic_common_reference<
          remove_cvref_t<T1>,
          remove_cvref_t<T2>,
          basic_common_reference_qual_gen<T1>::template qual,
          basic_common_reference_qual_gen<T2>::template qual
        >
    > {};

// https://eel.is/c++draft/meta.trans.other#6.3.3
// Otherwise, if `COND-RES(T1, T2)` is well-formed, then the member typedef type denotes that type.
template<typename T1, typename T2>
struct common_reference_tag_3 : has_typename_type<test_ternary_invoke<T1, T2>> {};


template<typename T1, typename T2>
using common_reference_tag = conditional_t<
    common_reference_tag_1<T1, T2>, std::integral_constant<int, 1>,
    common_reference_tag_2<T1, T2>, std::integral_constant<int, 2>,
    common_reference_tag_3<T1, T2>, std::integral_constant<int, 3>,

// https://eel.is/c++draft/meta.trans.other#6.3.4
// Otherwise, if common_type_t<T1, T2> is well-formed, then the member typedef type denotes that type.
// Solution: test both `std::common_type` and `preview::common_type`
    has_typename_type<std::common_type<T1, T2>>, std::integral_constant<int, 4>,
    has_typename_type<common_type<T1, T2>>, std::integral_constant<int, 5>,

    std::integral_constant<int, 0>
>;


template<typename T1, typename T2, int = common_reference_tag<T1, T2>::value>
struct common_reference_two {};

template<typename T1, typename T2>
struct common_reference_two<T1, T2, 1> {
  using type = typename simple_common_reference<T1, T2>::type;
};

template<typename T1, typename T2>
struct common_reference_two<T1, T2, 2> {
  using type = typename basic_common_reference<
      remove_cvref_t<T1>,
      remove_cvref_t<T2>,
      basic_common_reference_qual_gen<T1>::template qual,
      basic_common_reference_qual_gen<T2>::template qual>::type;
};

template<typename T1, typename T2>
struct common_reference_two<T1, T2, 3> {
  using type = test_ternary_invoke_t<T1, T2>;
};

template<typename T1, typename T2>
struct common_reference_two<T1, T2, 4> {
  using type = std::common_type_t<T1, T2>;
};

template<typename T1, typename T2>
struct common_reference_two<T1, T2, 5> {
  using type = common_type_t<T1, T2>;
};

template<bool /*true*/, typename T1, typename T2, typename... R>
struct common_reference_three : common_reference<typename common_reference<T1, T2>::type, R...> {};

template<typename T1, typename T2, typename... R>
struct common_reference_three<false, T1, T2, R...> {};

} // namespace detail

template<>
struct common_reference<> {};

template<typename T>
struct common_reference<T> {
  using type = T;
};

template<typename T1, typename T2>
struct common_reference<T1, T2> : detail::common_reference_two<T1, T2> {};

template<typename T1, typename T2, typename... R>
struct common_reference<T1, T2, R...>
    : detail::common_reference_three<
        has_typename_type<common_reference<T1, T2>>::value,
        T1,
        T2,
        R...
    > {};

template<typename... T>
using common_reference_t = typename common_reference<T...>::type;

} // namespace preview

#include "preview/__type_traits/basic_common_reference_std_specializations.h"

#endif // PREVIEW_TYPE_TRAITS_COMMON_REFERENCE_H_
