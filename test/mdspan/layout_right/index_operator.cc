//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <mdspan>

// Test default iteration:
//
// template<class... Indices>
//   constexpr index_type operator()(Indices...) const noexcept;
//
// Constraints:
//   * sizeof...(Indices) == extents_type::rank() is true,
//   * (is_convertible_v<Indices, index_type> && ...) is true, and
//   * (is_nothrow_constructible_v<index_type, Indices> && ...) is true.
//
// Preconditions:
//   * extents_type::index-cast(i) is a multidimensional index in extents_.

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "preview/mdspan.h"
#include "preview/span.h" // dynamic_extent

#include "gtest.h"
#include "../print_to.h"

#include "../ConvertibleToIntegral.h"

template<typename T, typename = void>
struct has_typename_index_type : std::false_type {};
template<typename T>
struct has_typename_index_type<T, preview::void_t<typename T::index_type>> : std::true_type {};

template<typename Mapping, typename... Indices>
struct operator_constraints
    : preview::conjunction<
        preview::is_invocable<Mapping&, Indices&...>,
        has_typename_index_type<Mapping>
    > {};

template<typename Mapping, typename ... Indices, std::enable_if_t<operator_constraints<Mapping, Indices...>::value, int> = 0>
constexpr bool check_operator_constraints(Mapping m, Indices ... idxs) {
  (void) m(idxs...);
  return true;
}

template<typename Mapping, typename ... Indices, std::enable_if_t<!operator_constraints<Mapping, Indices...>::value, int> = 0>
constexpr bool check_operator_constraints(Mapping, Indices ...) {
  return false;
}

template <class M, class T, class... Args, std::enable_if_t<(M::extents_type::rank() == sizeof...(Args)), int> = 0>
constexpr void iterate_right(M m, T& count, Args... args) {
  constexpr size_t r = sizeof...(Args);
  ASSERT_NOEXCEPT(m(args...));
  ASSERT_EQ(count, m(args...));
  count++;
}

template <class M, class T, class... Args, std::enable_if_t<(M::extents_type::rank() != sizeof...(Args)), int> = 0>
constexpr void iterate_right(M m, T& count, Args... args) {
  constexpr size_t r = sizeof...(Args);
  for (typename M::index_type i = 0; i < m.extents().extent(r); i++) {
    iterate_right(m, count, args..., i);
  }
}

template <class E, class... Args>
constexpr void test_iteration(Args... args) {
  using M = preview::layout_right::mapping<E>;
  M m(E(args...));

  typename E::index_type count = 0;
  iterate_right(m, count);
}

TEST(MdSpanLayoutRight, VERSIONED(index_operator)) {
  constexpr size_t D = preview::dynamic_extent;
  test_iteration<preview::extents<int>>();
  test_iteration<preview::extents<unsigned, D>>(1);
  test_iteration<preview::extents<unsigned, D>>(7);
  test_iteration<preview::extents<unsigned, 7>>();
  test_iteration<preview::extents<unsigned, 7, 8>>();
  test_iteration<preview::extents<signed char, D, D, D, D>>(1, 1, 1, 1);

  // Check operator constraint for number of arguments
  static_assert(check_operator_constraints(preview::layout_right::mapping<preview::extents<int, D>>(preview::extents<int, D>(1)), 0), "");
  static_assert(!check_operator_constraints(preview::layout_right::mapping<preview::extents<int, D>>(preview::extents<int, D>(1)), 0, 0), "");

  // Check operator constraint for convertibility of arguments to index_type
  static_assert(check_operator_constraints(preview::layout_right::mapping<preview::extents<int, D>>(preview::extents<int, D>(1)), IntType(0)), "");
  static_assert(!check_operator_constraints(preview::layout_right::mapping<preview::extents<unsigned, D>>(preview::extents<unsigned, D>(1)), IntType(0)), "");

  // Check operator constraint for no-throw-constructibility of index_type from arguments
  static_assert(!check_operator_constraints(preview::layout_right::mapping<preview::extents<unsigned char, D>>(preview::extents<unsigned char, D>(1)), IntType(0)), "");
}

TEST(MdSpanLayoutRight, VERSIONED(index_operator_large)) {
  constexpr size_t D = preview::dynamic_extent;
  // The large test iterates over ~10k loop indices.
  // With assertions enabled this triggered the maximum default limit
  // for steps in consteval expressions. Assertions roughly double the
  // total number of instructions, so this was already close to the maximum.
  test_iteration<preview::extents<int64_t, D, 8, D, D>>(7, 9, 10);
  test_iteration<preview::extents<int64_t, D, 8, 1, D>>(7, 10);
}
