//
// Created by YongGyu Lee on 5/16/24.
//

#ifndef PREVIEW_TYPE_TRAITS_COPY_TEMPLATE_H_
#define PREVIEW_TYPE_TRAITS_COPY_TEMPLATE_H_

#include "preview/__type_traits/type_identity.h"

namespace preview {

template<typename From, template<typename...> class To, template<typename, typename...> class Proj = type_identity_t>
struct copy_template;

template<template<typename...> class From, typename... T, template<typename...> class To, template<typename, typename...> class Proj>
struct copy_template<From<T...>, To, Proj> {
  using type = To<Proj<T>...>;
};

template<typename From, template<typename...> class To, template<typename, typename...> class Proj = type_identity_t>
using copy_template_t = typename copy_template<From, To, Proj>::type;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_COPY_TEMPLATE_H_
