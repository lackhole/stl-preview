//
// Created by yonggyulee on 2023/12/21.
//

#ifndef PREVIEW_CONCEPTS_REQUIRES_EXPRESSION_H_
#define PREVIEW_CONCEPTS_REQUIRES_EXPRESSION_H_

#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace detail {

template<template<typename, typename, typename...> class Check, typename Left, typename Right>
struct implicit_expression_check
    : conjunction<
        Check<Left, Right&>,
        Check<Left, Right&&>,
        Check<Left, const Right&>,
        Check<Left, const Right&&>
      > {};

} // namespace detail

template<template<typename, typename...> class Check, typename Operand, typename...>
struct requires_expression;

template<template<typename, typename...> class Check, typename Operand>
struct requires_expression<Check, Operand> : Check<Operand> {};

template<template<typename, typename, typename...> class Check, typename Left, typename Right>
struct requires_expression<Check, Left, Right> : Check<Left, Right> {};

// TODO: Add diagnostics for implicit requires expansion
//
//template<template<typename, typename, typename...> class Check, typename Left, typename Right>
//struct requires_expression<Check, Left, const Right&>
//    : detail::implicit_expression_check<Check, Left, Right> {};
//s
//template<template<typename, typename, typename...> class Check, typename Left, typename Right>
//struct requires_expression<Check, const Left&, const Right&>
//    : conjunction<
//        detail::implicit_expression_check<Check, Left&, Right>,
//        detail::implicit_expression_check<Check, Left&&, Right>,
//        detail::implicit_expression_check<Check, const Left&, Right>,
//        detail::implicit_expression_check<Check, const Left&&, Right>
//      > {};

//template<template<typename, typename...> class Check, typename Operand>
//struct requires_expression<Check, const Operand&> : conjunction<
//        Check<Operand&>,
//        Check<Operand&&>,
//        Check<const Operand&>,
//        Check<const Operand&&>
//      > {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_REQUIRES_EXPRESSION_H_
