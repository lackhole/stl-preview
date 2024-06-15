//
// Created by yonggyulee on 2023/10/27.
//

#ifndef PREVIEW_TUPLE_TUPLE_LIKE_H_
#define PREVIEW_TUPLE_TUPLE_LIKE_H_

#include <array>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/void_t.h"

namespace preview {

namespace internal {

template<typename T, typename = void>
struct has_tuple_size : std::false_type {};

template<typename T>
struct has_tuple_size<T, void_t<std::tuple_size<T>>> : std::true_type {};

template<typename T, std::size_t X, bool v = /* true */ has_tuple_size<T>::value>
struct tuple_size_equal_to : bool_constant<std::tuple_size<T>::value == X> {};

template<typename T, std::size_t X>
struct tuple_size_equal_to<T, X, false> : std::false_type {};

template<typename T, std::size_t X, bool v = /* true */ has_tuple_size<T>::value>
struct tuple_size_greater_than : bool_constant<(std::tuple_size<T>::value > X)> {};

template<typename T, std::size_t X>
struct tuple_size_greater_than<T, X, false> : std::false_type {};

template<typename T, std::size_t X, bool v = /* true */ has_tuple_size<T>::value>
struct tuple_size_less_than : bool_constant<(std::tuple_size<T>::value < X)> {};

template<typename T, std::size_t X>
struct tuple_size_less_than<T, X, false> : std::false_type {};

template<typename T>
struct tuple_like_uncvref : std::false_type {};


template<typename T, std::size_t N>
struct tuple_like_uncvref<std::array<T, N>> : std::true_type {};

template<typename T, typename U>
struct tuple_like_uncvref<std::pair<T, U>> : std::true_type {};

template<typename ...T>
struct tuple_like_uncvref<std::tuple<T...>> : std::true_type {};

} // namespace internal

template<typename T>
struct tuple_like : internal::tuple_like_uncvref<remove_cvref_t<T>> {};

template<typename T, std::size_t N>
struct sized_tuple_like
    : conjunction<
          tuple_like<T>,
          internal::tuple_size_equal_to<T, N>
      > {};

template<typename T>
using pair_like = sized_tuple_like<T, 2>;

} // namespace preview

#endif // PREVIEW_TUPLE_TUPLE_LIKE_H_
