//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_TYPE_TRAITS_DETAIL_TEST_TERNARY_H_
#define PREVIEW_TYPE_TRAITS_DETAIL_TEST_TERNARY_H_

#include <type_traits>

#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename T1, typename T2, typename = void>
struct test_ternary {};

template<typename T1, typename T2>
struct test_ternary<T1, T2, void_t<decltype(false ? std::declval<T1>() : std::declval<T2>())>> {
  using type = decltype(false ? std::declval<T1>() : std::declval<T2>());
};

template<typename T1, typename T2>
using test_ternary_t = typename test_ternary<T1, T2>::type;

// COND-RES
// https://eel.is/c++draft/meta.trans.other#3.4
template<typename T1, typename T2, typename = void>
struct test_ternary_invoke {};

template<typename T1, typename T2>
struct test_ternary_invoke<T1, T2, void_t<decltype(false ? std::declval<T1(&)()>()() : std::declval<T2(&)()>()())>> {
  using type = decltype(false ? std::declval<T1(&)()>()() : std::declval<T2(&)()>()());
};

template<typename T1, typename T2>
using test_ternary_invoke_t = typename test_ternary_invoke<T1, T2>::type;

} // namespace detail
} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_DETAIL_TEST_TERNARY_H_
