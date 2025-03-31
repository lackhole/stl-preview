
#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "gtest.h"
#include "mdspan/print_to.h"

#include "mdspan/ConvertibleToIntegral.h"

#include "preview/utility.h"

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


template <class Mapping, class... Indices, std::enable_if_t<operator_constraints<Mapping, Indices...>::value, int> = 0>
constexpr bool check_operator_constraints(Mapping m, Indices... idxs) {
  (void)m(idxs...);
  return true;
}

template <class Mapping, class... Indices, std::enable_if_t<!operator_constraints<Mapping, Indices...>::value, int> = 0>
constexpr bool check_operator_constraints(Mapping, Indices...) {
  return false;
}

template <class M, class... Args, std::enable_if_t<(-1 == static_cast<int>(M::extents_type::rank()) - 1 - static_cast<int>(sizeof...(Args))), int> = 0>
constexpr void iterate_stride(M m, const std::array<int, M::extents_type::rank()>& strides, Args... args) {
  constexpr int r = static_cast<int>(M::extents_type::rank()) - 1 - static_cast<int>(sizeof...(Args));
  ASSERT_NOEXCEPT(m(args...));
  std::size_t expected_val = 0;
  preview::sequence_for_each<M::extents_type::rank()>([&](auto i, auto&& arg) {
    expected_val += std::get<i>(arg) * strides[i];
  }, std::make_tuple(args...));

  ASSERT_EQ(expected_val, static_cast<std::size_t>(m(args...)));
}

template <class M, class... Args, std::enable_if_t<(-1 != static_cast<int>(M::extents_type::rank()) - 1 - static_cast<int>(sizeof...(Args))), int> = 0>
constexpr void iterate_stride(M m, const std::array<int, M::extents_type::rank()>& strides, Args... args) {
  constexpr int r = static_cast<int>(M::extents_type::rank()) - 1 - static_cast<int>(sizeof...(Args));
  for (typename M::index_type i = 0; i < m.extents().extent(r); i++) {
    iterate_stride(m, strides, i, args...);
  }
}

template <class E, class... Args>
constexpr void test_iteration(std::array<int, E::rank()> strides, Args... args) {
  using M = preview::layout_stride::mapping<E>;
  M m(E(args...), strides);

  iterate_stride(m, strides);
}

TEST(MdSpanLayoutStride, VERSIONED(index_operator)) {
  constexpr std::size_t D = preview::dynamic_extent;
  test_iteration<preview::extents<int>>(std::array<int, 0>{});
  test_iteration<preview::extents<unsigned, D>>(std::array<int, 1>{2}, 1);
  test_iteration<preview::extents<unsigned, D>>(std::array<int, 1>{3}, 7);
  test_iteration<preview::extents<unsigned, 7>>(std::array<int, 1>{4});
  test_iteration<preview::extents<unsigned, 7, 8>>(std::array<int, 2>{25, 3});
  test_iteration<preview::extents<signed char, D, D, D, D>>(std::array<int, 4>{1, 1, 1, 1}, 1, 1, 1, 1);

  // Check operator constraint for number of arguments
  static_assert(check_operator_constraints(
      preview::layout_stride::mapping<preview::extents<int, D>>(preview::extents<int, D>(1), preview::to_array({1})), 0), "");
  static_assert(!check_operator_constraints(
      preview::layout_stride::mapping<preview::extents<int, D>>(preview::extents<int, D>(1), preview::to_array({1})), 0, 0), "");

  // Check operator constraint for convertibility of arguments to index_type
  static_assert(check_operator_constraints(
      preview::layout_stride::mapping<preview::extents<int, D>>(preview::extents<int, D>(1), preview::to_array({1})), IntType(0)), "");
  static_assert(!check_operator_constraints(
      preview::layout_stride::mapping<preview::extents<unsigned, D>>(preview::extents<unsigned, D>(1), preview::to_array({1})), IntType(0)), "");

  // Check operator constraint for no-throw-constructibility of index_type from arguments
  static_assert(!check_operator_constraints(
      preview::layout_stride::mapping<preview::extents<unsigned char, D>>(preview::extents<unsigned char, D>(1), preview::to_array({1})), IntType(0)), "");
}

TEST(MdSpanLayoutStride, VERSIONED(index_operator_large)) {
  // The large test iterates over ~10k loop indices.
  // With assertions enabled this triggered the maximum default limit
  // for steps in consteval expressions. Assertions roughly double the
  // total number of instructions, so this was already close to the maximum.
  constexpr std::size_t D = preview::dynamic_extent;
  test_iteration<preview::extents<int64_t, D, 8, D, D>>(std::array<int, 4>{2000, 2, 20, 200}, 7, 9, 10);
  test_iteration<preview::extents<int64_t, D, 8, 1, D>>(std::array<int, 4>{2000, 20, 20, 200}, 7, 10);
}
