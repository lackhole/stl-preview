//
// Created by yonggyulee on 2024-09-10
//

#ifndef PREVIEW_TYPE_TRAITS_META_H_
#define PREVIEW_TYPE_TRAITS_META_H_

#include <cstddef>
#include <type_traits>

#include "preview/__utility/type_sequence.h"

namespace preview {
namespace meta {

template<template<typename...> class Constraint, typename... BoundArgs>
struct bind_front {
  template<typename... CallArgs>
  struct test : Constraint<BoundArgs..., CallArgs...> {};

  template<typename... CallArgs>
  struct test<type_sequence<CallArgs...>> : Constraint<BoundArgs..., CallArgs...> {};
};

template<template<typename...> class Constraint, typename... BoundArgs>
struct bind_back {
  template<typename... CallArgs>
  struct test : Constraint<CallArgs..., BoundArgs...> {};

  template<typename... CallArgs>
  struct test<type_sequence<CallArgs...>> : Constraint<CallArgs..., BoundArgs...> {};
};

template<template<typename...> class Constraint, typename First, typename... Rest>
struct bind_first : Constraint<First, Rest...> {};

template<template<typename...> class Constraint, typename First, typename... Rest>
struct bind_first<Constraint, First, type_sequence<Rest...>> : Constraint<First, Rest...> {};

template<template<typename...> class Constraint, typename TypeSequence>
struct apply;
template<template<typename...> class Constraint, typename... Args>
struct apply<Constraint, type_sequence<Args...>> : Constraint<Args...> {};

template<std::size_t I>
struct placeholder {};

template<typename T>
struct is_placeholder : std::false_type {};
template<std::size_t I>
struct is_placeholder<placeholder<I>> : std::true_type {};

namespace detail {

template<std::size_t I, typename Queue, typename To, typename Popped = type_sequence<>>
struct binder_replace_one;

template<std::size_t I, typename Q0, typename... Queued, typename To, typename... Popped>
struct binder_replace_one<I, type_sequence<Q0, Queued...>, To, type_sequence<Popped...>>
    : binder_replace_one<I, type_sequence<Queued...>, To, type_sequence<Popped..., Q0>> {};

template<std::size_t I, typename... Queued, typename To, typename... Popped>
struct binder_replace_one<I, type_sequence<placeholder<I>, Queued...>, To, type_sequence<Popped...>> {
  using type = type_sequence<Popped..., To, Queued...>;
};

template<std::size_t I, std::size_t N, typename TS, typename... CallArgs>
struct binder_replace_all_impl;

template<std::size_t N, typename... Args>
struct binder_replace_all_impl<N, N, type_sequence<Args...>> {
  using type = type_sequence<Args...>;
};
template<std::size_t I, std::size_t N, typename... T, typename U, typename... CallArgs>
struct binder_replace_all_impl<I, N, type_sequence<T...>, U, CallArgs...>
    : binder_replace_all_impl<I + 1, N, typename binder_replace_one<I, type_sequence<T...>, U>::type, CallArgs...> {};

template<std::size_t N, typename TypeSequence, typename... CallArgs>
using binder_replace_all_t = typename binder_replace_all_impl<0, N, TypeSequence, CallArgs...>::type;

} // namespace detail

template<template<typename...> class Constraint, typename... Args>
struct binder {
// private:
  using args = type_sequence<Args...>;
  static constexpr std::size_t placeholder_count = type_sequence_count_if<args, is_placeholder>::value;


  template<bool IsValid /* false */, typename... CallArgs>
  struct rebind_impl;

  template<typename... CallArgs>
  struct rebind_impl<true, CallArgs...>
      : apply<Constraint, detail::binder_replace_all_t<placeholder_count, args, CallArgs...>> {};

public:
  template<typename... CallArgs>
  struct rebind : rebind_impl<(sizeof...(CallArgs) == placeholder_count), CallArgs...> {};
};

} // namespace meta
} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_META_H_
