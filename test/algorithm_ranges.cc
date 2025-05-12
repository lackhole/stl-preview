#include "preview/algorithm.h"

#include <algorithm>
#include <cctype>
#include <complex>
#include <cstdlib>
#include <forward_list>
#include <iterator>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <random>
#include <thread>
#include <unordered_map>
#include <vector>

#include "preview/array.h"
#include "preview/core.h"
#include "preview/functional.h"
#include "preview/ranges.h"
#include "preview/span.h"
#include "preview/utility.h"

#include "test_utils.h"


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

TEST(VERSIONED(AlgorithmRanges), fill) {
  std::vector<int> v{0, 1, 2, 3, 4, 5};

  // set all elements to -1 using overload (1)
  ranges::fill(v.begin(), v.end(), -1);
  EXPECT_TRUE(ranges::equal(v, std::vector<int>(v.size(), -1)));

  // set all element to 10 using overload (2)
  ranges::fill(v, 10);
  EXPECT_TRUE(ranges::equal(v, views::repeat(10, v.size())));

  std::vector<std::complex<double>> nums{{1, 3}, {2, 2}, {4, 8}};
  ranges::fill(nums, {4, 2}); // T gets deduced
  EXPECT_TRUE(ranges::equal(nums, {{4, 2}, {4, 2}, {4, 2}}));
}

TEST(VERSIONED(AlgorithmRanges), fill_n) {
  constexpr auto n{8};

  std::vector<std::string> v(n, "AB");
  ranges::fill_n(v.begin(), n, "BA");
  EXPECT_TRUE(ranges::equal(v, views::repeat("BA"s, n)));

  std::vector<std::complex<double>> nums{{1, 3}, {2, 2}, {4, 8}};
  ranges::fill_n(nums.begin(), 2, {4, 2});
  EXPECT_TRUE(ranges::equal(nums, {{4, 2}, {4, 2}, {4, 8}}));

  std::vector<int> v2;
  ranges::fill_n(std::back_inserter(v2), n, 100);
  EXPECT_EQ(v2.size(), n);
  EXPECT_TRUE(ranges::equal(v2, views::repeat(100, n)));
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

TEST(VERSIONED(AlgorithmRanges), copy) {
  const auto source = views::iota(0, 10) | ranges::to<std::vector>();
  {
    std::vector<int> destination;
    ranges::copy(source.begin(), source.end(), std::back_inserter(destination));
    EXPECT_TRUE(ranges::equal(source, destination));
  }

  {
    std::vector<int> destination(source.size());
    ranges::copy(source.begin(), source.end(), destination.begin());
    EXPECT_TRUE(ranges::equal(source, destination));
  }

  {
    std::stringstream ss;
    ranges::copy(source, std::ostream_iterator<int>(ss, " "));
    EXPECT_EQ(ss.str(), "0 1 2 3 4 5 6 7 8 9 ");
  }

  {
    std::stringstream ss;
    ranges::copy_if(source, std::ostream_iterator<int>(ss, " "),
                    [](int x) { return (x % 2) == 1; });
    EXPECT_EQ(ss.str(), "1 3 5 7 9 ");
  }
}

TEST(VERSIONED(AlgorithmRanges), copy_n) {
  const preview::string_view in {"ABCDEFGH"};
  std::string out;

  ranges::copy_n(in.begin(), 4, std::back_inserter(out));
  EXPECT_EQ(out, "ABCD");

  out = "abcdefgh";
  const auto res = ranges::copy_n(in.begin(), 5, out.begin());
  EXPECT_EQ(*res.in, 'F');
  EXPECT_EQ(std::distance(std::begin(in), res.in), 5);
  EXPECT_EQ(*res.out, 'f');
  EXPECT_EQ(std::distance(std::begin(out), res.out), 5);
  EXPECT_EQ(out, "ABCDEfgh");

  std::vector<char> out_v(10, '0');
  ranges::copy_n(in.begin(), in.size(), out_v.begin());
  EXPECT_TRUE(ranges::equal(in, out_v | views::take(in.size())));
}

TEST(VERSIONED(AlgorithmRanges), copy_backward) {
  const auto src = {1, 2, 3, 4};
  std::vector<int> dst(src.size() + 2);
  ranges::copy_backward(src, dst.end());
  EXPECT_TRUE(ranges::equal(dst, {0, 0, 1, 2, 3, 4}));

  std::fill(dst.begin(), dst.end(), 0);
  const auto result = ranges::copy_backward(src.begin(), src.end() - 2, dst.end());
  EXPECT_TRUE(ranges::equal(dst, {0, 0, 0, 0, 1, 2}));
  EXPECT_EQ(std::distance(src.begin(), result.in), 2);
  EXPECT_EQ(std::distance(dst.begin(), result.out), 4);
}

TEST(VERSIONED(AlgorithmRanges), search_n) {
  static PREVIEW_CONSTEXPR_AFTER_CXX17 std::array<int, 10> nums = {1, 2, 2, 3, 4, 1, 2, 2, 2, 1};
  PREVIEW_CONSTEXPR_AFTER_CXX17 int count{3};
  PREVIEW_CONSTEXPR_AFTER_CXX17 int value{2};
  typedef int count_t, value_t;

  PREVIEW_CONSTEXPR_AFTER_CXX17 auto result1 = ranges::search_n(nums.begin(), nums.end(), count, value);
#if PREVIEW_CXX_VERSION >= 17
  static_assert( // found
      result1.size() == count &&
      std::distance(nums.begin(), result1.begin()) == 6 &&
      std::distance(nums.begin(), result1.end()) == 9
  );
#else
  EXPECT_EQ(result1.size(), count);
  EXPECT_EQ(std::distance(nums.begin(), result1.begin()), 6);
  EXPECT_EQ(std::distance(nums.begin(), result1.end()), 9);
#endif

  PREVIEW_CONSTEXPR_AFTER_CXX17 auto result2 = ranges::search_n(nums, count, value);
#if PREVIEW_CXX_VERSION >= 17
  static_assert( // found
      result2.size() == count &&
      std::distance(nums.begin(), result2.begin()) == 6 &&
      std::distance(nums.begin(), result2.end()) == 9
  );
#else
  EXPECT_EQ(result2.size(), count);
  EXPECT_EQ(std::distance(nums.begin(), result2.begin()), 6);
  EXPECT_EQ(std::distance(nums.begin(), result2.end()), 9);
#endif

  PREVIEW_CONSTEXPR_AFTER_CXX17 auto result3 = ranges::search_n(nums, count, value_t{5});
#if PREVIEW_CXX_VERSION >= 17
  static_assert( // not found
      result3.size() == 0 &&
      result3.begin() == result3.end() &&
      result3.end() == nums.end()
  );
#else
  EXPECT_EQ(result3.size(), 0);
  EXPECT_EQ(result3.begin(), result3.end());
  EXPECT_EQ(result3.end(), nums.end());
#endif

  PREVIEW_CONSTEXPR_AFTER_CXX17 auto result4 = ranges::search_n(nums, count_t{0}, value_t{1});
#if PREVIEW_CXX_VERSION >= 17
  static_assert( // not found
      result4.size() == 0 &&
      result4.begin() == result4.end() &&
      result4.end() == nums.begin()
  );
#else
  EXPECT_EQ(result4.size(), 0);
  EXPECT_EQ(result4.begin(), result4.end());
  EXPECT_EQ(result4.end(), nums.begin());
#endif

  constexpr char symbol{'B'};
  auto to_ascii = [](const int z) -> char { return 'A' + z - 1; };
  auto is_equ = [](const char x, const char y) { return x == y; };

//  TODO
//  std::cout << "Find a sub-sequence " << std::string(count, symbol) << " in the ";
//  ranges::transform(nums, std::ostream_iterator<char>(std::cout, ""), to_ascii);
//  std::cout << '\n';

  auto result5 = ranges::search_n(nums, count, symbol, is_equ, to_ascii);
  EXPECT_FALSE(result5.empty());
  EXPECT_EQ(ranges::distance(nums.begin(), result5.begin()), 6);

  std::vector<std::complex<double>> nums2{{4, 2}, {4, 2}, {1, 3}};
  auto it = ranges::search_n(nums2, 2, {4, 2});
  EXPECT_EQ(it.size(), 2);
}


#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

TEST(VERSIONED(AlgorithmRanges), minmax_element) {
  {
    // leftmost minimum   v
    const auto v  = {3, 9, 1, 4, 1, 2, 5, 9};
    // rightmost maximum               ^

#if PREVIEW_CXX_VERSION >= 17
    const auto [min, max] = ranges::minmax_element(v);
#else
    const auto p = ranges::minmax_element(v);
    const auto min = p.min;
    const auto max = p.max;
#endif
    EXPECT_EQ(min, v.begin() + 2);
    EXPECT_EQ(max, v.begin() + 7);
  }
  {
    static constexpr int v[] = {3, 9, 1, 4, 1, 2, 5, 9};
    constexpr auto minmax = ranges::minmax_element(v);
    static_assert(minmax.min == preview::ranges::begin(v) + 2, "");
    static_assert(minmax.max == preview::ranges::begin(v) + 7, "");
  }
  {
    const int v[]  = {3, 9, 1, 4, 1, 2, 5, 9};
    const auto r = v | views::take(5);
#if PREVIEW_CXX_VERSION >= 17
    const auto [min, max] = ranges::minmax_element(r);
#else
    const auto p = ranges::minmax_element(r);
    const auto min = p.min;
    const auto max = p.max;
#endif
    EXPECT_EQ(min, r.begin() + 2);
    EXPECT_EQ(max, r.begin() + 1);
  }
}

TEST(VERSIONED(AlgorithmRanges), minmax) {
  {
    constexpr auto v = preview::to_array({3, 1, 4, 1, 5, 9, 2, 6, 5});
#if PREVIEW_CXX_VERSION >= 17
    auto [min, max] = ranges::minmax(v);
#else
    auto p = ranges::minmax(v);
    auto min = p.min;
    auto max = p.max;
#endif
    EXPECT_EQ(min, 1);
    EXPECT_EQ(max, 9);
  }

  {
    std::random_device rd;
    std::mt19937_64 generator(rd());
    std::uniform_int_distribution<> distribution(0, 9);

    for (int i = 0; i < 100; ++i) {
      const int x1 = distribution(generator);
      const int x2 = distribution(generator);
#if PREVIEW_CXX_VERSION >= 17
      auto [min, max] = ranges::minmax(x1, x2);
#else
      auto p = ranges::minmax(x1, x2);
      auto min = p.min;
      auto max = p.max;
#endif
      ASSERT_EQ(min, (std::min)(x1, x2));
      ASSERT_EQ(max, (std::max)(x1, x2));
    }
  }
}


TEST(VERSIONED(AlgorithmRanges), starts_with) {
  EXPECT_FALSE(ranges::starts_with("hello world", "hello"));
  EXPECT_TRUE(ranges::starts_with("hello world", "hello"_sv));
  EXPECT_TRUE(ranges::starts_with("hello world"_sv, "hello"_sv));
#if PREVIEW_CXX_VERSION >= 17
  EXPECT_TRUE(ranges::starts_with("hello world", "hello"sv));
  EXPECT_TRUE(ranges::starts_with("hello world"sv, "hello"sv));
#endif

  EXPECT_TRUE(ranges::starts_with(std::vector<int>{1, 2, 3}, preview::span<const int>{1, 2, 3}));
  EXPECT_FALSE(ranges::starts_with(std::list<int>{1, 2, 3}, preview::to_array({1, 2, 3, 4})));

  EXPECT_TRUE(ranges::starts_with(views::iota(0) | views::take(10), views::iota(0, 5)));

  std::map<int, std::string> map{{1, "one"}, {2, "two"}, {3, "three"}};
  EXPECT_TRUE(ranges::starts_with(map, std::map<int, std::string>{{1, "one"}}));
  EXPECT_TRUE(ranges::starts_with(map, views::iota(1, 3), {}, preview::key));

  EXPECT_FALSE(ranges::starts_with(map, views::iota(1, 3), {}, preview::key, [](auto x) { return x * x; }));
}


TEST(VERSIONED(AlgorithmRanges), unique) {
  {
    std::vector<int> v{1, 2, 1, 1, 3, 3, 3, 4, 5, 4};
    const auto ret = ranges::unique(v);

    // v = { 1, 2, 1, 3, 4, 5, 4, x, x, x }
    EXPECT_EQ(v.begin() + 7, ret.begin());
    v.erase(ret.begin(), ret.end());
    EXPECT_TRUE(ranges::equal(v, {1, 2, 1, 3, 4, 5, 4}));

    ranges::sort(v);
    EXPECT_TRUE(ranges::equal(v, {1, 1, 2, 3, 4, 4, 5}));

    const auto ret2 = ranges::unique(v);
    // v = { 1, 2, 3, 4, 5, x, x, x }
    EXPECT_EQ(v.begin() + 5, ret2.begin());
    v.erase(ret2.begin(), ret2.end());
    EXPECT_TRUE(ranges::equal(v, {1, 2, 3, 4, 5}));
  }
  {
    std::vector<std::complex<int>> vc { {1, 1}, {-1, 2}, {-2, 3}, {2, 4}, {-3, 5} };
    const auto ret = ranges::unique(vc,
        [](int x, int y) { return std::abs(x) == std::abs(y); },
        [](std::complex<int> z) { return z.real(); });
    vc.erase(ret.begin(), ret.end());
    EXPECT_TRUE(ranges::equal(vc, {{1, 1}, {-2, 3}, {-3, 5}}));
  }
}


TEST(VERSIONED(AlgorithmRanges), unique_copy) {
  {
    std::string s1 = "The      string    with many       spaces!";
    std::string s2;
    ranges::unique_copy(s1.begin(), s1.end(), std::back_inserter(s2),
        [](char x, char y) { return x == ' ' && y == ' '; });
    EXPECT_EQ(s2, "The string with many spaces!"s);
  }
  {
    const auto v1 = {-1, +1, +2, -2, -3, +3, -3};
    std::list<int> v2;
    ranges::unique_copy(v1, std::back_inserter(v2), {}, &std::abs<int>);
    EXPECT_TRUE(ranges::equal(v2, {-1, +2, -3}));
  }
}


TEST(VERSIONED(AlgorithmRanges), move) {
  std::vector<std::string> v1 = {"hello", "world", "!"};
  std::vector<std::string> v2;
  ranges::move(v1.begin(), v1.end(), std::back_inserter(v2));
  ASSERT_EQ(v2.size(), 3);
  EXPECT_EQ(v2[0], "hello");
  EXPECT_EQ(v2[1], "world");
  EXPECT_EQ(v2[2], "!");
  EXPECT_TRUE(ranges::equal(v1, {""s, ""s, ""s}));

  // Move overlapping ranges to the left
  v2.emplace(v2.begin());
  EXPECT_TRUE(ranges::equal(v2, {""s, "hello"s, "world"s, "!"s}));
  ranges::move(v2.begin() + 1, v2.end(), v2.begin());
  EXPECT_TRUE(ranges::equal(v2, {"hello"s, "world"s, "!"s, ""s}));

  std::vector<std::thread> v3(10);
  std::vector<std::thread> v4;
  ranges::move(v3, std::back_inserter(v4));
  (void)v4;
}


TEST(VERSIONED(AlgorithmRanges), move_backward) {
  std::vector<std::string> v1 = {"hello", "world", "!"};
  std::vector<std::string> v2(3);
  ranges::move_backward(v1.begin(), v1.end(), v2.begin() + 3);
  EXPECT_EQ(v2[0], "hello");
  EXPECT_EQ(v2[1], "world");
  EXPECT_EQ(v2[2], "!");
  EXPECT_TRUE(ranges::equal(v1, {""s, ""s, ""s}));

  // Move overlapping ranges to the right
  v2.emplace_back();
  EXPECT_TRUE(ranges::equal(v2, {"hello"s, "world"s, "!"s, ""s}));
  ranges::move_backward(v2.begin(), v2.begin() + 3, v2.begin() + 4);
  EXPECT_TRUE(ranges::equal(v2, {""s, "hello"s, "world"s, "!"s}));

  std::vector<std::thread> v3(10);
  std::vector<std::thread> v4(10);
  ranges::move_backward(v3, v4.begin() + 3);
  (void)v4;
}
