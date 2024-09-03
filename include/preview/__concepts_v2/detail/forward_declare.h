//
// Created by yonggyulee on 2024. 9. 4.
//

#ifndef PREVIEW_CONCEPTS_V2_DETAIL_FORWARD_DECLARE_H_
#define PREVIEW_CONCEPTS_V2_DETAIL_FORWARD_DECLARE_H_

#include "preview/__type_traits/is_invocable.h"

namespace preview {
namespace concepts {

template<typename Derived, typename Bool>
struct concept_base;

template<template<typename...> class Test>
struct derived_from_template_tester {
  template<typename... Args>
  void operator()(Test<Args...>*) const {}
};

template<typename Derived, template<typename...> class Base>
struct derived_from_template : preview::is_invocable<derived_from_template_tester<Base>, Derived*> {};

template<typename T>
struct is_concept : derived_from_template<T, concept_base> {};

} // namespace concepts
} // namespace preview

#endif // PREVIEW_CONCEPTS_V2_DETAIL_FORWARD_DECLARE_H_
