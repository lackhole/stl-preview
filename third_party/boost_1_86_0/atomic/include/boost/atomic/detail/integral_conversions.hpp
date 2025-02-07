/*
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * Copyright (c) 2018 Andrey Semashev
 */
/*!
 * \file   atomic/detail/integral_conversions.hpp
 *
 * This header defines sign/zero extension and truncation utilities for Boost.Atomic. The tools assume two's complement signed integer representation.
 */

#ifndef BOOST_ATOMIC_DETAIL_INTEGRAL_CONVERSIONS_HPP_INCLUDED_
#define BOOST_ATOMIC_DETAIL_INTEGRAL_CONVERSIONS_HPP_INCLUDED_

#include <boost/atomic/detail/config.hpp>
#include <boost/atomic/detail/bitwise_cast.hpp>
#include <boost/atomic/detail/type_traits/integral_constant.hpp>
#include <boost/atomic/detail/header.hpp>

#include "preview/concepts.h"
#include "preview/type_traits.h"

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {
namespace atomics {
namespace detail {

template< typename Output, typename Input >
BOOST_FORCEINLINE Output zero_extend_impl(Input input, atomics::detail::true_type) BOOST_NOEXCEPT
{
    // Note: If we are casting with truncation or to the same-sized output, don't cause signed integer overflow by this chain of conversions
    return atomics::detail::bitwise_cast< Output >(static_cast< preview::make_unsigned_like_t< Output > >(
        static_cast< preview::make_unsigned_like_t< Input > >(input)));
}

template< typename Output, typename Input >
BOOST_FORCEINLINE Output zero_extend_impl(Input input, atomics::detail::false_type) BOOST_NOEXCEPT
{
    return static_cast< Output >(static_cast< preview::make_unsigned_like_t< Input > >(input));
}

//! Zero-extends or truncates (wraps) input operand to fit in the output type
template< typename Output, typename Input >
BOOST_FORCEINLINE Output zero_extend(Input input) BOOST_NOEXCEPT
{
    return atomics::detail::zero_extend_impl< Output >(input, atomics::detail::integral_constant< bool, preview::signed_integer_like< Output >::value >());
}

//! Truncates (wraps) input operand to fit in the output type
template< typename Output, typename Input >
BOOST_FORCEINLINE Output integral_truncate(Input input) BOOST_NOEXCEPT
{
    // zero_extend does the truncation
    return atomics::detail::zero_extend< Output >(input);
}

template< typename Output, typename Input >
BOOST_FORCEINLINE Output sign_extend_impl(Input input, atomics::detail::true_type) BOOST_NOEXCEPT
{
    return atomics::detail::integral_truncate< Output >(input);
}

template< typename Output, typename Input >
BOOST_FORCEINLINE Output sign_extend_impl(Input input, atomics::detail::false_type) BOOST_NOEXCEPT
{
    return static_cast< Output >(atomics::detail::bitwise_cast< preview::make_signed_like_t< Input > >(input));
}

//! Sign-extends or truncates (wraps) input operand to fit in the output type
template< typename Output, typename Input >
BOOST_FORCEINLINE Output sign_extend(Input input) BOOST_NOEXCEPT
{
    return atomics::detail::sign_extend_impl< Output >(input, atomics::detail::integral_constant< bool, sizeof(Output) <= sizeof(Input) >());
}

//! Sign-extends or truncates (wraps) input operand to fit in the output type
template< typename Output, typename Input >
BOOST_FORCEINLINE Output integral_extend(Input input, atomics::detail::true_type) BOOST_NOEXCEPT
{
    return atomics::detail::sign_extend< Output >(input);
}

//! Zero-extends or truncates (wraps) input operand to fit in the output type
template< typename Output, typename Input >
BOOST_FORCEINLINE Output integral_extend(Input input, atomics::detail::false_type) BOOST_NOEXCEPT
{
    return atomics::detail::zero_extend< Output >(input);
}

//! Sign- or zero-extends or truncates (wraps) input operand to fit in the output type
template< bool Signed, typename Output, typename Input >
BOOST_FORCEINLINE Output integral_extend(Input input) BOOST_NOEXCEPT
{
    return atomics::detail::integral_extend< Output >(input, atomics::detail::integral_constant< bool, Signed >());
}

} // namespace detail
} // namespace atomics
} // namespace boost

#include <boost/atomic/detail/footer.hpp>

#endif // BOOST_ATOMIC_DETAIL_INTEGRAL_CONVERSIONS_HPP_INCLUDED_
