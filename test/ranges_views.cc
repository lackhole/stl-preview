#include "preview/ranges.h"
#include "gtest.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <deque>
#include <forward_list>
#include <iterator>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <istream>
#include <iostream>
#include <sstream>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <valarray>
#include <vector>

#include "preview/algorithm.h"
#include "preview/concepts.h"
#include "preview/span.h"
#include "preview/string_view.h"

namespace ranges = preview::ranges;
namespace views = preview::views;

TEST(VERSIONED(RangesViews), empty_view) {
  ranges::empty_view<long> e;
  EXPECT_TRUE(ranges::empty(e)); // uses operator bool
  EXPECT_EQ(0, e.size());
  EXPECT_EQ(nullptr, e.data());
  EXPECT_EQ(nullptr, e.begin());
  EXPECT_EQ(nullptr, e.end());
  EXPECT_EQ(nullptr, e.cbegin());
  EXPECT_EQ(nullptr, e.cend());
}

TEST(VERSIONED(RangesViews), single_view) {
#if PREVIEW_CXX_VERSION >= 17
  constexpr ranges::single_view sv1{3.1415}; // uses (const T&) constructor
#else
  constexpr ranges::single_view<double> sv1{3.1415}; // uses (const T&) constructor
#endif
  EXPECT_TRUE(sv1);
  EXPECT_FALSE(sv1.empty());

  EXPECT_EQ(*sv1.data(), 3.1415);
  EXPECT_EQ(*sv1.begin(), 3.1415);
  EXPECT_EQ(sv1.size(), 1);
  EXPECT_EQ(std::distance(sv1.begin(), sv1.end()), 1);

  std::string str{"C++20"};
#if PREVIEW_CXX_VERSION >= 17
  ranges::single_view sv2{std::move(str)}; // uses (T&&) constructor
#else
  ranges::single_view<std::string> sv2{std::move(str)}; // uses (T&&) constructor
#endif
  EXPECT_EQ(*sv2.data(), "C++20");
  EXPECT_EQ(str, "");

  ranges::single_view<std::tuple<int, double, std::string>>
      sv3{preview::in_place, 42, 3.14, "Hello"}; // uses (std::in_place_t, Args&&... args)

  EXPECT_EQ(std::get<0>(sv3[0]), 42);
  EXPECT_EQ(std::get<1>(sv3[0]), 3.14);
  EXPECT_EQ(std::get<2>(sv3[0]), "Hello");
}

TEST(VERSIONED(RangesViews), iota_view) {
  struct Bound {
    int bound;
    bool operator==(int x) const { return x == bound; }
  };
  int answer[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

#if PREVIEW_CXX_VERSION >= 17
  EXPECT_TRUE(ranges::equal(ranges::iota_view{1, 10}, answer));
  for (int i : ranges::iota_view{1, 10}) { (void)i; }
#else
  EXPECT_TRUE(ranges::equal(ranges::iota_view<int, int>{1, 10}, answer));
  for (int i : ranges::iota_view<int, int>{1, 10}) { (void)i; }
#endif

  EXPECT_TRUE(ranges::equal(views::iota(1, 10), answer));
  for (int i : views::iota(1, 10)) { (void)i; }

  EXPECT_TRUE(ranges::equal(views::iota(1, Bound{10}), answer));
#if PREVIEW_CXX_VERSION >= 17
  for (int i : views::iota(1, Bound{10})) { (void)i; }
#endif

  EXPECT_TRUE(ranges::equal(views::iota(1) | views::take(9), answer));
#if PREVIEW_CXX_VERSION >= 17
  for (int i : views::iota(1) | views::take(9)) { (void)i; }
#endif

  int count = 0;
  ranges::for_each(views::iota(1, 10), [&](int i) {
    ++count;
    (void)i;
  });
  EXPECT_EQ(count, 9);
}

TEST(VERSIONED(RangesViews), basic_istream_view) {
  auto words = std::istringstream{"today is yesterday's tomorrow"};
#if PREVIEW_CXX_VERSION >= 17
  int counter = 0;
  for (const auto& s : views::istream<std::string>(words)) {
    if (counter == 0) EXPECT_EQ(s, "today");
    else if (counter == 1) EXPECT_EQ(s, "is");
    else if (counter == 2) EXPECT_EQ(s, "yesterday's");
    else if (counter == 3) EXPECT_EQ(s, "tomorrow");
    counter++;
  }
#endif

//#if !PREVIEW_ANDROID || (PREVIEW_CXX_VERSION < 17 || PREVIEW_NDK_VERSION_MAJOR >= 26)
//  auto floats = std::istringstream{"1.1  2.2\t3.3\v4.4\f55\n66\r7.7  8.8"};
//  std::ostringstream oss;
//  ranges::copy(
//      views::istream<float>(floats),
//      std::ostream_iterator<float>{oss, ", "}
//  );
//
//  EXPECT_EQ(oss.str(), "1.1, 2.2, 3.3, 4.4, 55, 66, 7.7, 8.8, ");
//#endif
}

TEST(VERSIONED(RangesViews), repeat_view) {
  using namespace preview::literals;

  // bounded overload
  for (auto s : views::repeat("C++"_sv, 3)) { (void)s; }
  EXPECT_TRUE(ranges::equal(
      views::repeat("C++"_sv, 3),
      {"C++"_sv, "C++"_sv, "C++"_sv}
  ));

  // unbounded overload
  for (auto s : views::repeat("I know that you know that"_sv) | views::take(3)) { (void)s; }
  EXPECT_TRUE(ranges::equal(
      views::repeat("I know that you know that"_sv) | views::take(3),
      {"I know that you know that"_sv, "I know that you know that"_sv, "I know that you know that"_sv}
  ));
}

TEST(VERSIONED(RangesViews), cartesian_product_view) {
  using namespace preview::literals;

  {
#if PREVIEW_CXX_VERSION >= 17
    static constexpr auto a = std::array{"Curiously"_sv, "Recurring"_sv, "Template"_sv, "Pattern"_sv};
    static constexpr auto v = ranges::cartesian_product_view(a[0], a[1], a[2], a[3]);
#else
    constexpr auto a = std::array<preview::string_view, 4>{"Curiously"_sv, "Recurring"_sv, "Template"_sv, "Pattern"_sv};
    constexpr auto v = ranges::cartesian_product_view<
        views::all_t<decltype(a[0])>,
        views::all_t<decltype(a[1])>,
        views::all_t<decltype(a[2])>,
        views::all_t<decltype(a[3])>
    >(a[0], a[1], a[2], a[3]);

#endif

    std::tuple<char const&, char const&, char const&, char const&> first{*v.begin()};
    EXPECT_EQ(std::get<0>(first), 'C');
    EXPECT_EQ(std::get<1>(first), 'R');
    EXPECT_EQ(std::get<2>(first), 'T');
    EXPECT_EQ(std::get<3>(first), 'P');

    std::tuple<char const&, char const&, char const&, char const&> last{*(v.end() - 1)};
    EXPECT_EQ(std::get<0>(last), 'y');
    EXPECT_EQ(std::get<1>(last), 'g');
    EXPECT_EQ(std::get<2>(last), 'e');
    EXPECT_EQ(std::get<3>(last), 'n');
  }

  {
    constexpr static auto w = {1};
    constexpr static auto x = {2, 3};
    constexpr static auto y = {4, 5, 6};
    constexpr static auto z = {7, 8, 9, 10, 11, 12, 13};

#if PREVIEW_CXX_VERSION >= 17
    constexpr auto v = ranges::cartesian_product_view(w, x, y, z);
    static_assert(v.size() == w.size() * x.size() * y.size() * z.size(), "");
#else
    auto v = ranges::cartesian_product_view<
        views::all_t<decltype(w)&>,
        views::all_t<decltype(x)&>,
        views::all_t<decltype(y)&>,
        views::all_t<decltype(z)&>
    >(w, x, y, z);
#endif
    EXPECT_EQ(v.size(), w.size() * x.size() * y.size() * z.size());
    EXPECT_EQ(v.size(), 42);
  }

  {
    const auto x = std::array<char, 2>{'A', 'B'};
    const auto y = std::vector<int> {1, 2, 3};
    const auto z = std::list<std::string>{"x", "y", "z", "w"};

    auto print = [](std::tuple<char const &, int const &, std::string const &>) {};

    for (auto const &tuple : views::cartesian_product(x, y, z)) {
      print(tuple);
    }

    auto t = [](auto a, auto b, auto c) -> std::tuple<char, int, std::string> {
      return std::make_tuple(a, b, c);
    };

    EXPECT_TRUE(ranges::equal(
        views::cartesian_product(x, y, z),
        {
            t('A', 1, "x"), t('A', 1, "y"), t('A', 1, "z"), t('A', 1, "w"),
            t('A', 2, "x"), t('A', 2, "y"), t('A', 2, "z"), t('A', 2, "w"),
            t('A', 3, "x"), t('A', 3, "y"), t('A', 3, "z"), t('A', 3, "w"),
            t('B', 1, "x"), t('B', 1, "y"), t('B', 1, "z"), t('B', 1, "w"),
            t('B', 2, "x"), t('B', 2, "y"), t('B', 2, "z"), t('B', 2, "w"),
            t('B', 3, "x"), t('B', 3, "y"), t('B', 3, "z"), t('B', 3, "w"),
        }
    ));
  }
}


TEST(VERSIONED(RangesViews), all_view) {
  std::vector<int> v{0, 1, 2, 3, 4, 5};
  for (int n : views::all(v) | views::take(2))
    std::cout << n << ' ';
  std::cout << '\n';

  static_assert(std::is_same<
      decltype(views::single(42)),
      ranges::single_view<int>
  >{}, "");

  static_assert(std::is_same<
      decltype(views::all(v)),
      ranges::ref_view<std::vector<int, std::allocator<int>>>
  >{}, "");

  int a[]{1, 2, 3, 4};
  static_assert(std::is_same<
      decltype(views::all(a)),
      ranges::ref_view<int[4]>
  >{}, "");

  static_assert(std::is_same<
#if PREVIEW_CXX_VERSION >= 17
      decltype(ranges::subrange{std::begin(a) + 1, std::end(a) - 1}),
#else
      decltype(ranges::make_subrange(std::begin(a) + 1, std::end(a) - 1)),
#endif
      ranges::subrange<int*, int*, ranges::subrange_kind(1)>
  >{}, "");
}


TEST(VERSIONED(RangesViews), ref_view) {
  using namespace preview::literals;

  const std::string s{"cosmos"};

#if PREVIEW_CXX_VERSION >= 17
  const ranges::take_view tv{s, 3};
  const ranges::ref_view rv{tv};
#else
  const ranges::take_view<views::all_t<decltype(s)&>> tv{s, 3};
  const ranges::ref_view<std::remove_reference_t<decltype(tv)>> rv{tv};
#endif

  EXPECT_FALSE(rv.empty());
  EXPECT_EQ(rv.size(), 3);
  EXPECT_EQ(*rv.begin(), 'c');
  EXPECT_EQ(*(rv.end() - 1), 's');
  EXPECT_EQ(std::strncmp(rv.data(), s.data(), 10), 0);
  EXPECT_EQ(rv.base().size(), 3);
  EXPECT_TRUE(ranges::equal(
      rv,
      "cos"_sv
  ));
}
