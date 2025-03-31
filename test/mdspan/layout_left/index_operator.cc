//
// Created by yonggyulee on 2024. 11. 6.
//

#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "gtest.h"
#include "../ConvertibleToIntegral.h"
#include "../print_to.h"

template<bool B, typename Mapping, typename... Indices>
struct operator_constraints_impl : std::false_type {};

template<typename Mapping, typename... Indices>
struct operator_constraints_impl<true, Mapping, Indices...>
    : std::is_same<typename Mapping::index_type, preview::invoke_result_t<Mapping&, Indices&...>> {};

template<typename Mapping, typename... Indices>
struct operator_constraints
    : operator_constraints_impl<preview::is_invocable<Mapping&, Indices&...>::value, Mapping, Indices...> {};

template<typename Mapping, typename... Indices, std::enable_if_t<operator_constraints<Mapping, Indices...>::value, int> = 0>
constexpr bool check_operator_constraints(Mapping m, Indices ... idxs) {
  (void) m(idxs...);
  return true;
}

template<typename Mapping, typename... Indices, std::enable_if_t<operator_constraints<Mapping, Indices...>::value == false, int> = 0>
constexpr bool check_operator_constraints(Mapping, Indices ...) {
  return false;
}

template <int r, class M, class T, class... Args, std::enable_if_t<(-1 == r), int> = 0>
constexpr void iterate_left(M m, T& count, Args... args) {
  ASSERT_NOEXCEPT(m(args...));
  ASSERT_EQ(count, m(args...));
  count++;
}

template <int r, class M, class T, class... Args, std::enable_if_t<(-1 != r), int> = 0>
constexpr void iterate_left(M m, T& count, Args... args) {
  for (typename M::index_type i = 0; i < m.extents().extent(r); i++) {
    iterate_left<r - 1>(m, count, i, args...);
  }
}

template <class E, class... Args>
constexpr void test_iteration(Args... args) {
  using M = preview::layout_left::mapping<E>;
  M m(E(args...));

  typename E::index_type count = 0;
  iterate_left<static_cast<int>(M::extents_type::rank()) - 1>(m, count);
}

TEST(MdSpanLayoutLeft, VERSIONED(index_operator)) {
  constexpr size_t D = preview::dynamic_extent;

  { // test
    test_iteration<preview::extents<int>>();
    test_iteration<preview::extents<unsigned, D>>(1);
    test_iteration<preview::extents<unsigned, D>>(7);
    test_iteration<preview::extents<unsigned, 7>>();
    test_iteration<preview::extents<unsigned, 7, 8>>();
    test_iteration<preview::extents<signed char, D, D, D, D>>(1, 1, 1, 1);

    // Check operator constraint for number of arguments
    static_assert(check_operator_constraints(preview::layout_left::mapping<preview::extents<int, D>>(preview::extents<int, D>(1)), 0), "");
    static_assert(!check_operator_constraints(preview::layout_left::mapping<preview::extents<int, D>>(preview::extents<int, D>(1)), 0, 0), "");

    // Check operator constraint for convertibility of arguments to index_type
    static_assert(check_operator_constraints(preview::layout_left::mapping<preview::extents<int, D>>(preview::extents<int, D>(1)), IntType(0)), "");
    static_assert(!check_operator_constraints(preview::layout_left::mapping<preview::extents<unsigned, D>>(preview::extents<unsigned, D>(1)), IntType(0)), "");

    // Check operator constraint for no-throw-constructibility of index_type from arguments
    static_assert(!check_operator_constraints(preview::layout_left::mapping<preview::extents<unsigned char, D>>(preview::extents<unsigned char, D>(1)), IntType(0)), "");
  }

  { // test_large
    test_iteration<preview::extents<int64_t, D, 8, D, D>>(7, 9, 10);
    test_iteration<preview::extents<int64_t, D, 8, 1, D>>(7, 10);
  }
}
