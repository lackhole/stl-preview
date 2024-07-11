#include <algorithm>
#include <complex>
#include <cstdlib>
#include <list>
#include <map>
#include <numeric>
#include <unordered_map>
#include <vector>

#include "preview/algorithm.h"
#include "preview/functional.h"
#include "preview/ranges.h"
#include "preview/utility.h"

#include "gtest.h"

namespace ranges = preview::ranges;
namespace views  = preview::views;
using namespace std::literals;
using namespace preview::literals;

TEST(VERSIONED(AlgorithmRanges), all_of) {
  std::vector<int> v = {1, 1, 1, 1};
  std::list<int> l = {2, 2, 2, 2, 3, 3, 3};
  std::map<int, int> m = {{1, 1}, {2, 2}, {3, 1}, {4, 2}};

  auto is_a = [](auto x) { return preview::bind_front(ranges::equal_to{}, x); };

  int arr[] = {1, 2, 3};
  auto p = arr;

  arr | views::take(4);
  EXPECT_TRUE ((ranges::all_of(v, is_a(1))));
  EXPECT_FALSE((ranges::all_of(l, is_a(2))));
  EXPECT_TRUE ((ranges::all_of(l | views::take(4), is_a(2))));
  EXPECT_TRUE ((ranges::all_of(l | views::drop(4), is_a(3))));
  EXPECT_FALSE((ranges::all_of(m | views::values , is_a(1))));
  EXPECT_TRUE ((ranges::all_of(m |
      views::filter([](const auto& p) { return p.first % 2; }) | views::values,
      is_a(1)
  )));
}

TEST(VERSIONED(AlgorithmRanges), none_of) {
  std::vector<int> v(10, 2);
  std::partial_sum(v.cbegin(), v.cend(), v.begin());

  auto is_odd = preview::bind_back(std::modulus<>{}, 2);
  auto is_even = preview::not_fn(is_odd);

  EXPECT_TRUE (ranges::none_of(v, is_odd));
  EXPECT_FALSE(ranges::none_of(v, is_even));
}

TEST(VERSIONED(AlgorithmRanges), any_of) {
  std::vector<int> v(10, 2);
  std::partial_sum(v.cbegin(), v.cend(), v.begin());
  auto DivisibleBy = [](int d) {
    return [d](int m) { return m % d == 0; };
  };

  EXPECT_TRUE (ranges::any_of(v, DivisibleBy(7)));
  EXPECT_TRUE (ranges::any_of(v, DivisibleBy(8)));
  EXPECT_FALSE(ranges::any_of(v, DivisibleBy(11)));
}

TEST(VERSIONED(AlgorithmRanges), for_each) {
  std::vector<int> nums {3, 4, 2, 8, 15, 267};
  auto print = [](const auto& n) { std::cout << ' ' << n; };
  ranges::for_each(preview::as_const(nums), print);
  print('\n');

  ranges::for_each(nums, [](int& n) { ++n; });
  ranges::for_each(nums.cbegin(), nums.cend(), print);
  EXPECT_TRUE((ranges::equal(nums, {4, 5, 3, 9, 16, 268})));

  struct Sum {
    void operator()(int n) { sum += n; }
    int sum {0};
  };
  // calls Sum::operator() for each number
#if PREVIEW_CXX_VERSION < 17
  auto i_s = ranges::for_each(nums.begin(), nums.end(), Sum());
  EXPECT_EQ(i_s.in, nums.end());
  EXPECT_EQ(i_s.fun.sum, 305);
#else
  auto [i, s] = ranges::for_each(nums.begin(), nums.end(), Sum());
  EXPECT_EQ(i, nums.end());
  EXPECT_EQ(s.sum, 305);
#endif

  using pair = std::pair<int, std::string>;
  std::vector<pair> pairs {{1,"one"}, {2,"two"}, {3,"tree"}};

  std::cout << "project the pair::first: ";
  ranges::for_each(pairs, print, [](const pair& p) { return p.first; });

  std::cout << "\n" "project the pair::second:";
  ranges::for_each(pairs, print, &pair::second);
  print('\n');
}

struct P {
  int first;
  char second;
  friend std::ostream& operator<<(std::ostream& os, const P& p)
  {
    return os << '{' << p.first << ",'" << p.second << "'}";
  }
};
TEST(VERSIONED(AlgorithmRanges), for_each_n) {
  auto print = [](preview::string_view name, auto const& v) {
    std::cout << name << ": ";
    auto n = v.size();
    for (const auto& e : v)
      std::cout << e << (--n ? ", " : "\n");
  };
  auto p_equal = [](const P& x, const P& y) {
    return x.first == y.first && x.second == y.second;
  };

  std::array<int, 5> a {1, 2, 3, 4, 5};

  // Negate first three numbers:
  ranges::for_each_n(a.begin(), 3, [](auto& n) { n *= -1; });
  EXPECT_TRUE(ranges::equal(a, {-1, -2, -3, 4, 5}));

  std::array<P, 4> s { P{1,'a'}, P{2, 'b'}, P{3, 'c'}, P{4, 'd'} };

  // Negate data members 'P::first' using projection:
  ranges::for_each_n(s.begin(), 2, [](auto& x) { x *= -1; }, &P::first);
  print("s", s);
  EXPECT_TRUE(ranges::equal(s, { P{-1,'a'}, P{-2, 'b'}, P{3, 'c'}, P{4, 'd'} }, p_equal));

  // Capitalize data members 'P::second' using projection:
  ranges::for_each_n(s.begin(), 3, [](auto& c) { c -= 'a'-'A'; }, &P::second);
  print("s", s);
  EXPECT_TRUE(ranges::equal(s, { P{-1,'A'}, P{-2, 'B'}, P{3, 'C'}, P{4, 'd'} }, p_equal));
}

TEST(VERSIONED(AlgorithmRanges), contains) {
  constexpr auto haystack = std::array<int, 5>{3, 1, 4, 1, 5};
  constexpr auto needle = std::array<int, 3>{1, 4, 1};
  constexpr auto bodkin = std::array<int, 3>{2, 5, 2};

#if PREVIEW_CXX_VERSION >= 17
  static_assert(
      ranges::contains(haystack, 4) &&
          !ranges::contains(haystack, 6) &&
          ranges::contains_subrange(haystack, needle) &&
          !ranges::contains_subrange(haystack, bodkin)
  );
#else
  EXPECT_TRUE (ranges::contains(haystack, 4));
  EXPECT_FALSE(ranges::contains(haystack, 6));
  EXPECT_TRUE (ranges::contains_subrange(haystack, needle));
  EXPECT_FALSE(ranges::contains_subrange(haystack, bodkin));
#endif

  constexpr std::array<std::complex<double>, 3> nums{{{1, 2}, {3, 4}, {5, 6}}};
#if PREVIEW_CXX_VERSION >= 17
  static_assert(ranges::contains(nums, {3, 4}));
#else
  EXPECT_TRUE(ranges::contains(nums, {3, 4}));
#endif

  static_assert(preview::is_invocable<decltype(ranges::contains), void, int>::value == false, "");
  static_assert(preview::is_invocable<decltype(ranges::contains), int, int>::value == false, "");
}
