/*
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * Copyright (c) 2020 Andrey Semashev
 */
/*!
 * \file   atomic/detail/memory_order_utils.hpp
 *
 * This header contains utilities related to memory order constants.
 */

#ifndef BOOST_ATOMIC_DETAIL_MEMORY_ORDER_UTILS_HPP_INCLUDED_
#define BOOST_ATOMIC_DETAIL_MEMORY_ORDER_UTILS_HPP_INCLUDED_

#include <atomic>

#include <boost/atomic/detail/config.hpp>
#include <boost/atomic/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {
namespace atomics {
namespace detail {

BOOST_FORCEINLINE BOOST_CONSTEXPR std::memory_order deduce_failure_order(std::memory_order order) noexcept
{
    return order == std::memory_order_acq_rel ? std::memory_order_acquire : (order == std::memory_order_release ? std::memory_order_relaxed : order);
}

BOOST_FORCEINLINE BOOST_CONSTEXPR bool cas_failure_order_must_not_be_stronger_than_success_order(std::memory_order success_order, std::memory_order failure_order) noexcept
{
    // 15 == (std::memory_order_seq_cst | std::memory_order_consume), see std::memory_order.hpp
    // Given the enum values we can test the strength of memory order requirements with this single condition.
    return (static_cast< unsigned int >(failure_order) & 15u) <= (static_cast< unsigned int >(success_order) & 15u);
}

} // namespace detail
} // namespace atomics
} // namespace boost

#include <boost/atomic/detail/footer.hpp>

#endif // BOOST_ATOMIC_DETAIL_MEMORY_ORDER_UTILS_HPP_INCLUDED_
