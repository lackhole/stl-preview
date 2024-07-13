#include "preview/algorithm.h"

#include <algorithm>
#include <complex>
#include <cstdlib>
#include <forward_list>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

#include "preview/core.h"
#include "preview/functional.h"
#include "preview/ranges.h"
#include "preview/utility.h"

#include "gtest.h"


#if PREVIEW_CXX_VERSION >= 20 && \
    (!defined(__clang_major__) || (defined(__clang_major__) && __clang_major__ > 11)) && \
    (!defined(PREVIEW_NDK_VERSION_MAJOR) || PREVIEW_NDK_VERSION_MAJOR >= 26)
#define PREVIEW_FULL_CXX20_SUPPORT 1
#define PREVIEW_CONSTEXPR_AFTER_CXX20_STRICT constexpr
#else
#define PREVIEW_FULL_CXX20_SUPPORT 0
#define PREVIEW_CONSTEXPR_AFTER_CXX20_STRICT
#endif

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

TEST(VERSIONED(AlgorithmRanges), count) {
  std::vector<int> v{1, 2, 3, 4, 4, 3, 7, 8, 9, 10};

  // determine how many integers in a std::vector match a target value.
  EXPECT_EQ((ranges::count(v.begin(), v.end(), 3)), 2);
  EXPECT_EQ((ranges::count(v.begin(), v.end(), 5)), 0);

  // use a lambda expression to count elements divisible by 3.
  auto count1 = ranges::count_if(v.begin(), v.end(), [](int i){ return i % 3 == 0; }); // NDK < 21 guard
  EXPECT_EQ(count1, 3);

  // use a lambda expression to count elements divisible by 11.
  auto count2 = ranges::count_if(v, [](int i){ return i % 11 == 0; });
  EXPECT_EQ(count2, 0);

  std::vector<std::complex<double>> nums{{4, 2}, {1, 3}, {4, 2}};
  EXPECT_EQ((ranges::count(nums, {4, 2})), 2);
}

PREVIEW_CONSTEXPR_AFTER_CXX17 preview::string_view mirror_ends(const preview::string_view in) {
  const auto end = ranges::mismatch(in, in | views::reverse).in1;
  return {in.cbegin(), end};
}
TEST(VERSIONED(AlgorithmRanges), mismatch) {
  EXPECT_EQ(mirror_ends("abXYZba"), "ab"_sv);
  EXPECT_EQ(mirror_ends("abca"), "a"_sv);
  EXPECT_EQ(mirror_ends("ABBA"), "ABBA"_sv);
  EXPECT_EQ(mirror_ends("level"), "level"_sv);
  EXPECT_EQ("123"_sv, mirror_ends("123!@#321"));
  EXPECT_EQ("radar"_sv, mirror_ends("radar"));
}

TEST(VERSIONED(AlgorithmRanges), find) {
  {
    struct folk_info {
      unsigned uid;
      std::string name, position;
    };

    std::vector<folk_info> folks {
      {0, "Ana", "dev"},
      {1, "Bob", "devops"},
      {2, "Eve", "ops"}
    };

    const auto who{"Eve"};
    auto it = ranges::find(folks, who, &folk_info::name);
    EXPECT_NE(it, folks.end());
    EXPECT_EQ(it->uid, 2);
    EXPECT_EQ(it->name, "Eve");
    EXPECT_EQ(it->position, "ops");
  }

  const int n1 = 3;
  const int n2 = 5;
  const auto v = {4, 1, 3, 2};

  EXPECT_NE(ranges::find(v, n1), v.end());
  EXPECT_EQ(ranges::find(v.begin(), v.end(), n2), v.end());

  auto is_even = [](int x) { return x % 2 == 0; };
  {
    auto result = ranges::find_if(v.begin(), v.end(), is_even);
    EXPECT_TRUE(result != v.end() && *result == 4);
  }
  {
    auto result = ranges::find_if_not(v, is_even);
    EXPECT_TRUE(result != v.end() && *result == 1);
  }

  auto divides_13 = [](int x) { return x % 13 == 0; };
  EXPECT_EQ(ranges::find_if(v, divides_13), v.end());
  EXPECT_EQ(*ranges::find_if_not(v.begin(), v.end(), divides_13), 4);

  std::vector<std::complex<double>> nums{{4, 2}};
  EXPECT_EQ(ranges::find(nums, {4, 2}), nums.begin());
}

TEST(VERSIONED(AlgorithmRanges), find_last) {
  constexpr static auto v = {1, 2, 3, 1, 2, 3, 1, 2};

  {
    PREVIEW_CONSTEXPR_AFTER_CXX20_STRICT auto i1 = ranges::find_last(v.begin(), v.end(), 3);
    PREVIEW_CONSTEXPR_AFTER_CXX20_STRICT auto i2 = ranges::find_last(v, 3);
#if PREVIEW_FULL_CXX20_SUPPORT
    static_assert(ranges::distance(v.begin(), i1.begin()) == 5);
    static_assert(ranges::distance(v.begin(), i2.begin()) == 5);
    static_assert(i1.end() == v.end());
    static_assert(i2.end() == v.end());
#else
    EXPECT_EQ(ranges::distance(v.begin(), i1.begin()), 5);
    EXPECT_EQ(ranges::distance(v.begin(), i2.begin()), 5);
    EXPECT_EQ(i1.end(), v.end());
    EXPECT_EQ(i2.end(), v.end());
#endif
  }
  {
    PREVIEW_CONSTEXPR_AFTER_CXX20_STRICT auto i1 = ranges::find_last(v.begin(), v.end(), -3);
    PREVIEW_CONSTEXPR_AFTER_CXX20_STRICT auto i2 = ranges::find_last(v, -3);
#if PREVIEW_FULL_CXX20_SUPPORT
    static_assert(i1.begin() == v.end());
    static_assert(i2.begin() == v.end());
    static_assert(i1.end() == v.end());
    static_assert(i2.end() == v.end());
#else
    EXPECT_EQ(i1.begin(), v.end());
    EXPECT_EQ(i2.begin(), v.end());
    EXPECT_EQ(i1.end(), v.end());
    EXPECT_EQ(i2.end(), v.end());
#endif
  }

  auto abs = [](int x) { return x < 0 ? -x : x; };

  {
    auto pred = [](int x) { return x == 3; };
    PREVIEW_CONSTEXPR_AFTER_CXX20_STRICT auto i1 = ranges::find_last_if(v.begin(), v.end(), pred, abs);
    PREVIEW_CONSTEXPR_AFTER_CXX20_STRICT auto i2 = ranges::find_last_if(v, pred, abs);
#if PREVIEW_FULL_CXX20_SUPPORT
    static_assert(ranges::distance(v.begin(), i1.begin()) == 5);
    static_assert(ranges::distance(v.begin(), i2.begin()) == 5);
    static_assert(i1.end() == v.end());
    static_assert(i2.end() == v.end());
#else
    EXPECT_EQ(ranges::distance(v.begin(), i1.begin()), 5);
    EXPECT_EQ(ranges::distance(v.begin(), i2.begin()), 5);
    EXPECT_EQ(i1.end(), v.end());
    EXPECT_EQ(i2.end(), v.end());
#endif
  }
  {
    auto pred = [](int x) { return x == -3; };
    PREVIEW_CONSTEXPR_AFTER_CXX20_STRICT auto i1 = ranges::find_last_if(v.begin(), v.end(), pred, abs);
    PREVIEW_CONSTEXPR_AFTER_CXX20_STRICT auto i2 = ranges::find_last_if(v, pred, abs);
#if PREVIEW_FULL_CXX20_SUPPORT
    static_assert(i1.begin() == v.end());
    static_assert(i2.begin() == v.end());
    static_assert(i1.end() == v.end());
    static_assert(i2.end() == v.end());
#else
    EXPECT_EQ(i1.begin(), v.end());
    EXPECT_EQ(i2.begin(), v.end());
    EXPECT_EQ(i1.end(), v.end());
    EXPECT_EQ(i2.end(), v.end());
#endif
  }

  {
    auto pred = [](int x) { return x == 1 || x == 2; };
    PREVIEW_CONSTEXPR_AFTER_CXX20_STRICT auto i1 = ranges::find_last_if_not(v.begin(), v.end(), pred, abs);
    PREVIEW_CONSTEXPR_AFTER_CXX20_STRICT auto i2 = ranges::find_last_if_not(v, pred, abs);
#if PREVIEW_FULL_CXX20_SUPPORT
    static_assert(ranges::distance(v.begin(), i1.begin()) == 5);
    static_assert(ranges::distance(v.begin(), i2.begin()) == 5);
    static_assert(i1.end() == v.end());
    static_assert(i2.end() == v.end());
#else
    EXPECT_EQ(ranges::distance(v.begin(), i1.begin()), 5);
    EXPECT_EQ(ranges::distance(v.begin(), i2.begin()), 5);
    EXPECT_EQ(i1.end(), v.end());
    EXPECT_EQ(i2.end(), v.end());
#endif
  }
  {
    auto pred = [](int x) { return x == 1 or x == 2 or x == 3; };
    PREVIEW_CONSTEXPR_AFTER_CXX20_STRICT auto i1 = ranges::find_last_if_not(v.begin(), v.end(), pred, abs);
    PREVIEW_CONSTEXPR_AFTER_CXX20_STRICT auto i2 = ranges::find_last_if_not(v, pred, abs);
#if PREVIEW_FULL_CXX20_SUPPORT
    static_assert(i1.begin() == v.end());
    static_assert(i2.begin() == v.end());
    static_assert(i1.end() == v.end());
    static_assert(i2.end() == v.end());
#else
    EXPECT_EQ(i1.begin(), v.end());
    EXPECT_EQ(i2.begin(), v.end());
    EXPECT_EQ(i1.end(), v.end());
    EXPECT_EQ(i2.end(), v.end());
#endif
  }

  using P = std::pair<preview::string_view, int>;
  std::forward_list<P> list {
      {"one", 1}, {"two", 2}, {"three", 3},
      {"one", 4}, {"two", 5}, {"three", 6},
  };
  auto cmp_one = preview::bind_back(ranges::equal_to{}, "one");
  auto cmp_zero = preview::bind_back(ranges::equal_to{}, "zero");

  {
    // find latest element that satisfy the comparator, and projecting pair::first
    const auto subrange = ranges::find_last_if(list, cmp_one, &P::first);
    EXPECT_EQ(subrange.begin(), ranges::next(list.begin(), 3));
    EXPECT_EQ(subrange.end(), list.end());
  }

  {
    // find latest element that satisfy the comparator, and projecting pair::first
    const auto subrange = ranges::find_last_if(list, cmp_zero, &P::first);
    EXPECT_EQ(subrange.begin(), list.end());
    EXPECT_EQ(subrange.end(), list.end());
  }

  const auto i3 = ranges::find_last(list, {"three", 3}); // (2) C++26
  EXPECT_EQ(i3.begin()->first, "three");
  EXPECT_EQ(i3.begin()->second, 3);
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
