//
// Created by yonggyulee on 2024-09-10
//

#ifndef PREVIEW_TYPE_TRAITS_META_H_
#define PREVIEW_TYPE_TRAITS_META_H_

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

} // namespace meta
} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_META_H_
