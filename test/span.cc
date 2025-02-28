#include "preview/span.h"
#include "test_utils.h"

#include <array>
#include <list>
#include <vector>

#include "preview/algorithm.h"
#include "preview/array.h"
#include "preview/ranges.h"

namespace ranges = preview::ranges;
namespace views = preview::views;

TEST(VERSIONED(Span), ctor) {
  int c[]{1, 2, 3};
  EXPECT_TRUE(ranges::equal(preview::span<const int>(c), c)); // constructs from array

  const std::array<int, 3> a{4, 5, 6};
  EXPECT_TRUE(ranges::equal(preview::span<const int>(a), a)); // constructs from std::array

  std::vector<int> v{7, 8, 9};
  EXPECT_TRUE(ranges::equal(preview::span<const int>(v), v)); // constructs from std::vector

  EXPECT_TRUE(ranges::equal(preview::span<const int>{0, 1, 2}, {0, 1, 2})); // constructs from initializer_list
}

TEST(VERSIONED(Span), operator_assign) {
  std::array<int, 6> a1;
  std::array<int, 6> a2;
  a1.fill(3);
  a2.fill(4);

#if PREVIEW_CXX_VERSION < 17
  auto s1 = preview::span<int, 6>(a1);
  auto s2 = preview::span<int, 6>(a2);
#else
  auto s1 = preview::span(a1);
  auto s2 = preview::span(a2);
#endif
  EXPECT_TRUE(ranges::equal(s1, views::repeat(3, 6)));
  EXPECT_TRUE(ranges::equal(s2, views::repeat(4, 6)));

  // Check that assignment performs a shallow copy.
  s1 = s2;
  EXPECT_EQ(s1.data(), s2.data());
  EXPECT_EQ(s1.size(), s2.size());

  ranges::fill(s1, 5);
  // s2 is also 'updated' since s1 and s2 point to the same data
  EXPECT_TRUE(ranges::equal(s1, s2));
//  print("s1", s1);
//  print("s2", s2);
//  print();

  int a3[]{1, 2, 3, 4};
  int a4[]{2, 3, 4, 5};
  int a5[]{3, 4, 5};

  preview::span<int, preview::dynamic_extent> dynamic_1{a3};
  preview::span<int, preview::dynamic_extent> dynamic_2{a4, 3};
  preview::span<int, 4> static_1{a3};
  preview::span<int, 4> static_2{a4};
  preview::span<int, 3> static_3{a5};


  dynamic_1 = dynamic_2; // OK
  dynamic_1 = static_1;  // OK
  EXPECT_FALSE_TYPE(std::is_assignable<decltype((static_1)), decltype((dynamic_1))>); // ERROR: no match for 'operator='
  static_1 = static_2;   // OK: same extents = 4
  EXPECT_FALSE_TYPE(std::is_assignable<decltype((static_1)), decltype((static_3))>);  // ERROR: different extents: 4 and 3
}

#if PREVIEW_CXX_VERSION >= 17

TEST(VERSIONED(Span), deduction_guides) {
  int a[]{1, 2, 3, 4, 5};

  preview::span s1{std::begin(a), std::end(a)}; // guide (1)
  EXPECT_EQ(sizeof s1, sizeof(std::size_t) * 2);
  EXPECT_EQ(s1.extent, preview::dynamic_extent);

  preview::span s2{std::begin(a), 3}; // guide (1)
  EXPECT_EQ(sizeof s2, sizeof(std::size_t) * 2);
  EXPECT_EQ(s2.extent, preview::dynamic_extent);

  preview::span s3{std::begin(a), std::integral_constant<std::size_t, 2>{}}; // guide (1)
  EXPECT_EQ(sizeof s3, sizeof(std::size_t) * 1);
  EXPECT_EQ(s3.extent, 2);

  preview::span s4{a}; // guide (2)
  EXPECT_EQ(sizeof s4, sizeof(std::size_t) * 1);
  EXPECT_EQ(s4.extent, 5);

  preview::span<int> s5{a}; // does not use a guide, makes a dynamic span
  EXPECT_EQ(sizeof s5, sizeof(std::size_t) * 2);
  EXPECT_EQ(s5.extent, preview::dynamic_extent);

  std::array arr{6, 7, 8};
  preview::span s6{arr}; // guide (3)
  EXPECT_EQ(sizeof s6, sizeof(std::size_t) * 1);
  EXPECT_EQ(s6.extent, 3);
  s6[0] = 42; // OK, element_type is 'int'

  const std::array arr2{9, 10, 11};
  preview::span s7{arr2}; // guide (4)
  EXPECT_EQ(sizeof s7, sizeof(std::size_t) * 1);
  EXPECT_EQ(s7.extent, 3);
  // s7[0] = 42; // Error: element_type is 'const int'

  std::vector v{66, 69, 99};
  preview::span s8{v}; // guide (5)
  EXPECT_EQ(sizeof s8, sizeof(std::size_t) * 2);
  EXPECT_EQ(s8.extent, preview::dynamic_extent);
}

#endif // C++17

#if PREVIEW_CXX_VERSION >= 20

#include <span>

TEST(VERSIONED(Span), std_span_compat) {
  const auto arr = preview::to_array({1, 2, 3, 4, 5});

  static_assert(std::is_convertible_v<preview::span<int>, preview::span<int>> ==
                std::is_convertible_v<preview::span<int>,     std::span<int>>, "");
  static_assert(std::is_convertible_v<preview::span<unsigned int>, preview::span<int>> ==
                std::is_convertible_v<preview::span<unsigned int>,     std::span<int>>, "");
  static_assert(std::is_convertible_v<preview::span<int>, preview::span<unsigned int>> ==
                std::is_convertible_v<preview::span<int>,     std::span<unsigned int>>, "");

  static_assert(std::is_convertible_v<preview::span<int, 5>, preview::span<int>> ==
                std::is_convertible_v<preview::span<int, 5>,     std::span<int>>, "");
  static_assert(std::is_convertible_v<preview::span<unsigned int, 5>, preview::span<int>> ==
                std::is_convertible_v<preview::span<unsigned int, 5>,     std::span<int>>, "");
  static_assert(std::is_convertible_v<preview::span<int, 5>, preview::span<unsigned int>> ==
                std::is_convertible_v<preview::span<int, 5>,     std::span<unsigned int>>, "");

  static_assert(std::is_convertible_v<preview::span<int>, preview::span<int, 5>> ==
                std::is_convertible_v<preview::span<int>,     std::span<int, 5>>, "");
  static_assert(std::is_convertible_v<preview::span<unsigned int>, preview::span<int, 5>> ==
                std::is_convertible_v<preview::span<unsigned int>,     std::span<int, 5>>, "");
  static_assert(std::is_convertible_v<preview::span<int>, preview::span<unsigned int, 5>> ==
                std::is_convertible_v<preview::span<int>,     std::span<unsigned int, 5>>, "");

  static_assert(std::is_convertible_v<preview::span<int, 5>, preview::span<int, 5>> ==
                std::is_convertible_v<preview::span<int, 5>,     std::span<int, 5>>, "");
  static_assert(std::is_convertible_v<preview::span<unsigned int, 5>, preview::span<int, 5>> ==
                std::is_convertible_v<preview::span<unsigned int, 5>,     std::span<int, 5>>, "");
  static_assert(std::is_convertible_v<preview::span<int, 5>, preview::span<unsigned int, 5>> ==
                std::is_convertible_v<preview::span<int, 5>,     std::span<unsigned int, 5>>, "");

  static_assert(preview::is_explicitly_convertible_v<preview::span<int>, preview::span<int>> ==
                preview::is_explicitly_convertible_v<preview::span<int>,     std::span<int>>, "");
  static_assert(preview::is_explicitly_convertible_v<preview::span<unsigned int>, preview::span<int>> ==
                preview::is_explicitly_convertible_v<preview::span<unsigned int>,     std::span<int>>, "");
  static_assert(preview::is_explicitly_convertible_v<preview::span<int>, preview::span<unsigned int>> ==
                preview::is_explicitly_convertible_v<preview::span<int>,     std::span<unsigned int>>, "");

  static_assert(preview::is_explicitly_convertible_v<preview::span<int, 5>, preview::span<int>> ==
                preview::is_explicitly_convertible_v<preview::span<int, 5>,     std::span<int>>, "");
  static_assert(preview::is_explicitly_convertible_v<preview::span<unsigned int, 5>, preview::span<int>> ==
                preview::is_explicitly_convertible_v<preview::span<unsigned int, 5>,     std::span<int>>, "");
  static_assert(preview::is_explicitly_convertible_v<preview::span<int, 5>, preview::span<unsigned int>> ==
                preview::is_explicitly_convertible_v<preview::span<int, 5>,     std::span<unsigned int>>, "");

  static_assert(preview::is_explicitly_convertible_v<preview::span<int>, preview::span<int, 5>> ==
                preview::is_explicitly_convertible_v<preview::span<int>,     std::span<int, 5>>, "");
  static_assert(preview::is_explicitly_convertible_v<preview::span<unsigned int>, preview::span<int, 5>> ==
                preview::is_explicitly_convertible_v<preview::span<unsigned int>,     std::span<int, 5>>, "");
  static_assert(preview::is_explicitly_convertible_v<preview::span<int>, preview::span<unsigned int, 5>> ==
                preview::is_explicitly_convertible_v<preview::span<int>,     std::span<unsigned int, 5>>, "");

  static_assert(preview::is_explicitly_convertible_v<preview::span<int, 5>, preview::span<int, 5>> ==
                preview::is_explicitly_convertible_v<preview::span<int, 5>,     std::span<int, 5>>, "");
  static_assert(preview::is_explicitly_convertible_v<preview::span<unsigned int, 5>, preview::span<int, 5>> ==
                preview::is_explicitly_convertible_v<preview::span<unsigned int, 5>,     std::span<int, 5>>, "");
  static_assert(preview::is_explicitly_convertible_v<preview::span<int, 5>, preview::span<unsigned int, 5>> ==
                preview::is_explicitly_convertible_v<preview::span<int, 5>,     std::span<unsigned int, 5>>, "");

  static_assert(std::is_constructible_v<preview::span<int>, preview::span<int>> ==
                std::is_constructible_v<    std::span<int>, preview::span<int>>, "");
  static_assert(std::is_constructible_v<preview::span<unsigned int>, preview::span<int>> ==
                std::is_constructible_v<    std::span<unsigned int>, preview::span<int>>, "");
  static_assert(std::is_constructible_v<preview::span<int>, preview::span<unsigned int>> ==
                std::is_constructible_v<    std::span<int>, preview::span<unsigned int>>, "");

  static_assert(std::is_constructible_v<preview::span<int, 5>, preview::span<int>> ==
                std::is_constructible_v<    std::span<int, 5>, preview::span<int>>, "");
  static_assert(std::is_constructible_v<preview::span<unsigned int, 5>, preview::span<int>> ==
                std::is_constructible_v<    std::span<unsigned int, 5>, preview::span<int>>, "");
  static_assert(std::is_constructible_v<preview::span<int, 5>, preview::span<unsigned int>> ==
                std::is_constructible_v<    std::span<int, 5>, preview::span<unsigned int>>, "");

  static_assert(std::is_constructible_v<preview::span<int>, preview::span<int, 5>> ==
                std::is_constructible_v<    std::span<int>, preview::span<int, 5>>, "");
  static_assert(std::is_constructible_v<preview::span<unsigned int>, preview::span<int, 5>> ==
                std::is_constructible_v<    std::span<unsigned int>, preview::span<int, 5>>, "");
  static_assert(std::is_constructible_v<preview::span<int>, preview::span<unsigned int, 5>> ==
                std::is_constructible_v<    std::span<int>, preview::span<unsigned int, 5>>, "");

  static_assert(std::is_constructible_v<preview::span<int, 5>, preview::span<int, 5>> ==
                std::is_constructible_v<    std::span<int, 5>, preview::span<int, 5>>, "");
  static_assert(std::is_constructible_v<preview::span<unsigned int, 5>, preview::span<int, 5>> ==
                std::is_constructible_v<    std::span<unsigned int, 5>, preview::span<int, 5>>, "");
  static_assert(std::is_constructible_v<preview::span<int, 5>, preview::span<unsigned int, 5>> ==
                std::is_constructible_v<    std::span<int, 5>, preview::span<unsigned int, 5>>, "");
}

#endif // C++17
