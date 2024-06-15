//
// Created by yonggyulee on 2023/12/21.
//

#ifndef PREVIEW_CONCEPTS_IMPLICIT_EXPRESSION_CHECK_H_
#define PREVIEW_CONCEPTS_IMPLICIT_EXPRESSION_CHECK_H_

#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace detail {

template<template<typename, typename, typename...> class Check, typename Left, typename Right>
struct implicit_expression_check_impl
    : conjunction<
        Check<Left, Right&>,
        Check<Left, Right&&>,
        Check<Left, const Right&>,
        Check<Left, const Right&&>
      > {};

} // namespace detail

template<template<typename, typename...> class Check, typename Operand, typename...>
struct implicit_expression_check;

template<template<typename, typename, typename...> class Check, typename Left, typename Right>
struct implicit_expression_check<Check, Left, Right> : Check<Left, Right> {};

template<template<typename, typename, typename...> class Check, typename Left, typename Right>
struct implicit_expression_check<Check, Left, const Right&>
    : detail::implicit_expression_check_impl<Check, Left, Right> {};

template<template<typename, typename, typename...> class Check, typename Left, typename Right>
struct implicit_expression_check<Check, const Left&, const Right&>
    : conjunction<
        detail::implicit_expression_check_impl<Check, Left&, Right>,
        detail::implicit_expression_check_impl<Check, Left&&, Right>,
        detail::implicit_expression_check_impl<Check, const Left&, Right>,
        detail::implicit_expression_check_impl<Check, const Left&&, Right>
      > {};

template<template<typename, typename...> class Check, typename Operand>
struct implicit_expression_check<Check, Operand> : Check<Operand> {};

template<template<typename, typename...> class Check, typename Operand>
struct implicit_expression_check<Check, const Operand&> : conjunction<
        Check<Operand&>,
        Check<Operand&&>,
        Check<const Operand&>,
        Check<const Operand&&>
      > {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_IMPLICIT_EXPRESSION_CHECK_H_
