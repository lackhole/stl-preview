//
// Created by yonggyulee on 2024. 11. 3.
//

#include "preview/mdspan.h"

#include <cstddef>
#include <ostream>

#include "../../test_utils.h"
#include "../print_to.h"

#include <tuple>

#include "preview/array.h"
#include "preview/string_view.h"

#include "CtorTestCombinations.h"


template<typename T>
class MdSpanExtentsCtor : public testing::Test {};
using MdSpanExtentsCtorTypes = ::testing::Types<
    std::pair<int, int>,
    std::pair<int, size_t>,
    std::pair<unsigned, int>,
    std::pair<signed char, size_t>,
    std::pair<long long, unsigned>,
    std::pair<size_t, int>,
    std::pair<size_t, size_t>,
    std::pair<int, IntType>,
    std::pair<signed char, IntType>
>;
TYPED_TEST_SUITE(MdSpanExtentsCtor, MdSpanExtentsCtorTypes);

struct DefaultCtorTest {
  template <class E, class AllExtents, class Extents, std::size_t... Indices>
  static testing::AssertionResult test_construction(AllExtents all_ext, Extents, std::index_sequence<Indices...>) {
    ASSERT_NOEXCEPT(E{});
    // This function gets called twice: once with Extents being just the dynamic ones, and once with all the extents specified.
    // We only test during the all extent case, since then Indices is the correct number. This allows us to reuse the same
    // testing machinery used in other constructor tests.

    if PREVIEW_CONSTEXPR_AFTER_CXX17 (sizeof...(Indices) == E::rank()) {
      // Need to construct new expected values, replacing dynamic values with 0
      std::array<typename AllExtents::value_type, E::rank()> expected_exts{
          ((E::static_extent(Indices) == preview::dynamic_extent)
           ? typename AllExtents::value_type(0)
           : all_ext[Indices])...};
      return test_runtime_observers(E{}, expected_exts);
    }
    return testing::AssertionSuccess();
  }
};


VERSIONED_TYPE_TEST(MdSpanExtentsCtor, defaults) {
  using T = typename TypeParam::first_type;
  using TArg = typename TypeParam::second_type;
  using Test = DefaultCtorTest;
  constexpr std::size_t D = preview::dynamic_extent;

  EXPECT_TRUE((test_construction<preview::extents<T>, Test>(std::array<TArg, 0>{})));

  EXPECT_TRUE((test_construction<preview::extents<T, 3>, Test>(std::array<TArg, 1>{3})));
  EXPECT_TRUE((test_construction<preview::extents<T, D>, Test>(std::array<TArg, 1>{3})));

  EXPECT_TRUE((test_construction<preview::extents<T, 3, 7>, Test>(std::array<TArg, 2>{3, 7})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, D>, Test>(std::array<TArg, 2>{3, 7})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 7>, Test>(std::array<TArg, 2>{3, 7})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D>, Test>(std::array<TArg, 2>{3, 7})));

  EXPECT_TRUE((test_construction<preview::extents<T, 3, 7, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, 7, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, D, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 7, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, D, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 7, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));

  EXPECT_TRUE((test_construction<preview::extents<T, 1, 2, 3, 4, 5, 6, 7, 8, 9>, Test>(std::array<TArg, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 2, 3, D, 5, D, 7, D, 9>, Test>(std::array<TArg, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D, D, D, D, D, D, D, D>, Test>(std::array<TArg, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9})));
}

struct ArrayCtorTest {
  template <class E, class T, size_t N, class Extents, size_t... Indices, std::enable_if_t<(N == E::rank_dynamic()), int> = 0>
  static testing::AssertionResult test_construction(std::array<T, N> all_ext, Extents ext, std::index_sequence<Indices...>) {
    ASSERT_NOEXCEPT(E(ext));
    auto result = test_implicit_construction_call<E>(ext, all_ext);
    if (!result) {
      return result;
    }

    return test_runtime_observers(E(ext), all_ext);
  }
  template <class E, class T, size_t N, class Extents, size_t... Indices, std::enable_if_t<(N != E::rank_dynamic()), int> = 0>
  static testing::AssertionResult test_construction(std::array<T, N> all_ext, Extents ext, std::index_sequence<Indices...>) {
    ASSERT_NOEXCEPT(E(ext));
    return test_runtime_observers(E(ext), all_ext);
  }
};

template <class E>
struct implicit_construction {
  bool value;
  implicit_construction(E) : value(true) {}
  template <class T>
  implicit_construction(T) : value(false) {}
};

VERSIONED_TYPE_TEST(MdSpanExtentsCtor, from_array) {
  using T = typename TypeParam::first_type;
  using TArg = typename TypeParam::second_type;
  using Test = ArrayCtorTest;
  constexpr size_t D = preview::dynamic_extent;

  EXPECT_TRUE((test_construction<preview::extents<T>, Test>(std::array<TArg, 0>{})));

  EXPECT_TRUE((test_construction<preview::extents<T, 3>, Test>(std::array<TArg, 1>{3})));
  EXPECT_TRUE((test_construction<preview::extents<T, D>, Test>(std::array<TArg, 1>{3})));

  EXPECT_TRUE((test_construction<preview::extents<T, 3, 7>, Test>(std::array<TArg, 2>{3, 7})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, D>, Test>(std::array<TArg, 2>{3, 7})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 7>, Test>(std::array<TArg, 2>{3, 7})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D>, Test>(std::array<TArg, 2>{3, 7})));

  EXPECT_TRUE((test_construction<preview::extents<T, 3, 7, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, 7, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, D, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 7, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, D, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 7, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));

  EXPECT_TRUE((test_construction<preview::extents<T, 1, 2, 3, 4, 5, 6, 7, 8, 9>, Test>(std::array<TArg, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 2, 3, D, 5, D, 7, D, 9>, Test>(std::array<TArg, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D, D, D, D, D, D, D, D>, Test>(std::array<TArg, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9})));
}

TEST(MdSpanExtentsCtor, VERSIONED(from_array_assertions)) {
  constexpr size_t D = preview::dynamic_extent;
  using E            = preview::extents<int, 1, D, 3, D>;

  // check can't construct from too few arguments
  static_assert(!std::is_constructible<E, std::array<int, 1>>::value, "extents constructible from illegal arguments");
  // check can't construct from rank_dynamic < #args < rank
  static_assert(!std::is_constructible<E, std::array<int, 3>>::value, "extents constructible from illegal arguments");
  // check can't construct from too many arguments
  static_assert(!std::is_constructible<E, std::array<int, 5>>::value, "extents constructible from illegal arguments");

  // test implicit construction fails from span and array if all extents are given
  auto a5 = preview::to_array({3, 4, 5, 6, 7});
  // check that explicit construction works, i.e. no error
  static_assert(std::is_constructible< preview::extents<int, D, D, 5, D, D>, decltype(a5)>::value,
                "extents unexpectedly not constructible");
  // check that implicit construction doesn't work
  EXPECT_FALSE((implicit_construction<preview::extents<int, D, D, 5, D, D>>(a5).value));

  // test construction fails from types not convertible to index_type but convertible to other integer types
  static_assert(std::is_convertible<IntType, int>::value, "Test helper IntType unexpectedly not convertible to int");
  static_assert(!std::is_constructible< preview::extents<unsigned long, D>, std::array<IntType, 1>>::value,
                "extents constructible from illegal arguments");

  // index_type is not nothrow constructible
  static_assert(std::is_convertible<IntType, unsigned char>::value, "");
  static_assert(std::is_convertible<const IntType&, unsigned char>::value, "");
  static_assert(!std::is_nothrow_constructible<unsigned char, const IntType&>::value, "");
  static_assert(!std::is_constructible<preview::dextents<unsigned char, 2>, std::array<IntType, 2>>::value, "");

  // convertible from non-const to index_type but not  from const
  static_assert(std::is_convertible<IntTypeNC, int>::value, "");
  static_assert(!std::is_convertible<const IntTypeNC&, int>::value, "");
  static_assert(std::is_nothrow_constructible<int, IntTypeNC>::value, "");
  static_assert(!std::is_constructible<preview::dextents<int, 2>, std::array<IntTypeNC, 2>>::value, "");
}


struct IntegralCtorTest {
  template <class E, class AllExtents, class Extents, size_t... Indices>
  static testing::AssertionResult test_construction(AllExtents all_ext, Extents ext, std::index_sequence<Indices...>) {
    // construction from indices
    // Split to impl since some implementation doesn't define operator[] as noexcept
    return test_construction_impl<E>(std::move(all_ext), ext[Indices]...);
  }

  template <class E, class AllExtents, typename... Extents>
  static testing::AssertionResult test_construction_impl(AllExtents all_ext, Extents... exts) {
    ASSERT_NOEXCEPT(E(exts...));
    return test_runtime_observers(E(exts...), all_ext);
  }
};

VERSIONED_TYPE_TEST(MdSpanExtentsCtor, from_integral) {
  using T = typename TypeParam::first_type;
  using TArg = typename TypeParam::second_type;
  using Test = IntegralCtorTest;
  constexpr size_t D = preview::dynamic_extent;

  EXPECT_TRUE((test_construction<preview::extents<T>, Test>(std::array<TArg, 0>{})));

  EXPECT_TRUE((test_construction<preview::extents<T, 3>, Test>(std::array<TArg, 1>{3})));
  EXPECT_TRUE((test_construction<preview::extents<T, D>, Test>(std::array<TArg, 1>{3})));

  EXPECT_TRUE((test_construction<preview::extents<T, 3, 7>, Test>(std::array<TArg, 2>{3, 7})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, D>, Test>(std::array<TArg, 2>{3, 7})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 7>, Test>(std::array<TArg, 2>{3, 7})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D>, Test>(std::array<TArg, 2>{3, 7})));

  EXPECT_TRUE((test_construction<preview::extents<T, 3, 7, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, 7, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, D, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 7, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, D, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 7, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));

  EXPECT_TRUE((test_construction<preview::extents<T, 1, 2, 3, 4, 5, 6, 7, 8, 9>, Test>(std::array<TArg, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 2, 3, D, 5, D, 7, D, 9>, Test>(std::array<TArg, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D, D, D, D, D, D, D, D>, Test>(std::array<TArg, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9})));
}

TEST(MdSpanExtentsCtor, VERSIONED(from_integral_assertions)) {
  constexpr size_t D = preview::dynamic_extent;
  using E            = preview::extents<int, 1, D, 3, D>;

  // check can't construct from too few arguments
  static_assert(!std::is_constructible<E, int>::value, "extents constructible from illegal arguments");
  // check can't construct from rank_dynamic < #args < rank
  static_assert(!std::is_constructible<E, int, int, int>::value, "extents constructible from illegal arguments");
  // check can't construct from too many arguments
  static_assert(!std::is_constructible<E, int, int, int, int, int>::value, "extents constructible from illegal arguments");

  // test construction fails from types not convertible to index_type but convertible to other integer types
  static_assert(std::is_convertible<IntType, int>::value, "Test helper IntType unexpectedly not convertible to int");
  static_assert(!std::is_constructible< preview::extents<unsigned long, D>, IntType>::value,
                "extents constructible from illegal arguments");
}


struct SpanCtorTest {
  template <class E, class T, size_t N, class Extents, size_t... Indices, std::enable_if_t<N == E::rank_dynamic(), int> = 0>
  static testing::AssertionResult test_construction(std::array<T, N> all_ext, Extents ext, std::index_sequence<Indices...>) {
    ASSERT_NOEXCEPT(E(ext));
    using ext_span = preview::span<typename Extents::value_type, ext.size()>;

    auto result = test_implicit_construction_call<E>(ext_span(ext), all_ext);
    if (!result) {
      return result;
    }
    return test_runtime_observers(E(ext_span(ext)), all_ext);
  }
  template <class E, class T, size_t N, class Extents, size_t... Indices, std::enable_if_t<N != E::rank_dynamic(), int> = 0>
  static testing::AssertionResult test_construction(std::array<T, N> all_ext, Extents ext, std::index_sequence<Indices...>) {
    ASSERT_NOEXCEPT(E(ext));
    using ext_span = preview::span<typename Extents::value_type, ext.size()>;
    return test_runtime_observers(E(ext_span(ext)), all_ext);
  }
};

VERSIONED_TYPE_TEST(MdSpanExtentsCtor, from_span) {
  using T = typename TypeParam::first_type;
  using TArg = typename TypeParam::second_type;
  using Test = SpanCtorTest;
  constexpr size_t D = preview::dynamic_extent;

  EXPECT_TRUE((test_construction<preview::extents<T>, Test>(std::array<TArg, 0>{})));

  EXPECT_TRUE((test_construction<preview::extents<T, 3>, Test>(std::array<TArg, 1>{3})));
  EXPECT_TRUE((test_construction<preview::extents<T, D>, Test>(std::array<TArg, 1>{3})));

  EXPECT_TRUE((test_construction<preview::extents<T, 3, 7>, Test>(std::array<TArg, 2>{3, 7})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, D>, Test>(std::array<TArg, 2>{3, 7})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 7>, Test>(std::array<TArg, 2>{3, 7})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D>, Test>(std::array<TArg, 2>{3, 7})));

  EXPECT_TRUE((test_construction<preview::extents<T, 3, 7, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, 7, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, D, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 7, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D, D>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, 3, D, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 7, 9>, Test>(std::array<TArg, 3>{3, 7, 9})));

  EXPECT_TRUE((test_construction<preview::extents<T, 1, 2, 3, 4, 5, 6, 7, 8, 9>, Test>(std::array<TArg, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, 2, 3, D, 5, D, 7, D, 9>, Test>(std::array<TArg, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9})));
  EXPECT_TRUE((test_construction<preview::extents<T, D, D, D, D, D, D, D, D, D>, Test>(std::array<TArg, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9})));
}

TEST(MdSpanExtentsCtor, VERSIONED(from_span_assertions)) {
  constexpr size_t D = preview::dynamic_extent;
  using E            = preview::extents<int, 1, D, 3, D>;

  // check can't construct from too few arguments
  static_assert(!std::is_constructible<E, preview::span<int, 1>>::value, "extents constructible from illegal arguments");
  // check can't construct from rank_dynamic < #args < rank
  static_assert(!std::is_constructible<E, preview::span<int, 3>>::value, "extents constructible from illegal arguments");
  // check can't construct from too many arguments
  static_assert(!std::is_constructible<E, preview::span<int, 5>>::value, "extents constructible from illegal arguments");

  // test implicit construction fails from span and array if all extents are given
  auto a5 = preview::to_array({3, 4, 5, 6, 7});
  preview::span<int, 5> s5(a5.data(), 5);
  // check that explicit construction works, i.e. no error
  static_assert(std::is_constructible< preview::extents<int, D, D, 5, D, D>, decltype(s5)>::value,
                "extents unexpectedly not constructible");
  // check that implicit construction doesn't work
  assert((implicit_construction<preview::extents<int, D, D, 5, D, D>>(s5).value == false));

  // test construction fails from types not convertible to index_type but convertible to other integer types
  static_assert(std::is_convertible<IntType, int>::value, "Test helper IntType unexpectedly not convertible to int");
  static_assert(!std::is_constructible< preview::extents<unsigned long, D>, preview::span<IntType, 1>>::value,
                "extents constructible from illegal arguments");

  // index_type is not nothrow constructible
  static_assert(std::is_convertible<IntType, unsigned char>::value, "");
  static_assert(std::is_convertible<const IntType&, unsigned char>::value, "");
  static_assert(!std::is_nothrow_constructible<unsigned char, const IntType&>::value, "");
  static_assert(!std::is_constructible<preview::dextents<unsigned char, 2>, preview::span<IntType, 2>>::value, "");

  // convertible from non-const to index_type but not  from const
  static_assert(std::is_convertible<IntTypeNC, int>::value, "");
  static_assert(!std::is_convertible<const IntTypeNC&, int>::value, "");
  static_assert(std::is_nothrow_constructible<int, IntTypeNC>::value, "");
  static_assert(!std::is_constructible<preview::dextents<int, 2>, preview::span<IntTypeNC, 2>>::value, "");
}
