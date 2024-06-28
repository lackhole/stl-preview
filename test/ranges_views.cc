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
#include <numeric>
#include <sstream>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <valarray>
#include <vector>

#include "preview/algorithm.h"
#include "preview/concepts.h"
#include "preview/functional.h"
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


TEST(VERSIONED(RangesViews), owning_view) {
  using namespace std::literals;
  using namespace preview::literals;

#if PREVIEW_CXX_VERSION > 17
  ranges::owning_view ov{"cosmos"s}; // the deduced type of R is std::string;
#else
  ranges::owning_view<std::string> ov{"cosmos"s};
#endif

  EXPECT_FALSE(ov.empty());
  EXPECT_EQ(ov.size(), 6);
  EXPECT_EQ(ov.size(), ov.base().size());
  EXPECT_EQ(ov.front(), 'c');
  EXPECT_EQ(ov.front(), *ov.begin());
  EXPECT_EQ(ov.back(), 's');
  EXPECT_EQ(ov.back(), *(ov.end() - 1));
  EXPECT_EQ(ov.data(), ov.base());

  // typically equal to sizeof(R)
  EXPECT_EQ(sizeof ov, sizeof(std::string));
  for (const char ch : ov) { (void)ch; }

  ranges::owning_view<std::string> ov2;
  EXPECT_TRUE(ov2.empty());

//  ov2 = ov; // compile-time error: copy assignment operator is deleted
  EXPECT_FALSE_TYPE(std::is_copy_assignable<decltype(ov2)>);

  ov2 = std::move(ov); // OK
  EXPECT_EQ(ov2.size(), 6);
  EXPECT_EQ(ov.size(), 0);
}


TEST(VERSIONED(RangesViews), filter_view) {
  auto even = [](int i) { return 0 == i % 2; };
  auto square = [](int i) { return i * i; };

  for (int i : views::iota(0, 6)
      | views::filter(even)
      | views::transform(square)) { (void)i; }

  EXPECT_TRUE(ranges::equal(
      views::iota(0, 6) | views::filter(even) | views::transform(square),
      {0, 4, 16}
  ));
}

char rot13a(const char x, const char a) {
  return a + (x - a + 13) % 26;
}

char rot13(const char x) {
  if ('Z' >= x && x >= 'A')
    return rot13a(x, 'A');

  if ('z' >= x && x >= 'a')
    return rot13a(x, 'a');

  return x;
}

TEST(VERSIONED(RangesViews), transform_view) {
  using namespace std::literals;
  { // ctor
    auto atan1term = ranges::views::transform(
        [](int n) { return ((n % 2) ? -1 : 1) * 1.0 / (2 * n + 1); }
    );

    std::vector<double> out;
    for (const int iterations : {1, 2, 3, 4, 5, 10, 100, 1000, 1'000'000})
    {
      auto seq = views::iota(0, iterations) | atan1term;
      const double accum = std::accumulate(seq.begin(), seq.end(), 0.0);
      out.push_back(4 * accum);
    }
    EXPECT_DOUBLE_EQ(out[0], 4.000000000000000);
    EXPECT_DOUBLE_EQ(out[1], 2.666666666666667);
    EXPECT_DOUBLE_EQ(out[2], 3.466666666666667);
    EXPECT_DOUBLE_EQ(out[3], 2.895238095238096);
    EXPECT_DOUBLE_EQ(out[4], 3.339682539682540);
    EXPECT_DOUBLE_EQ(out[5], 3.041839618929403);
    EXPECT_DOUBLE_EQ(out[6], 3.131592903558554);
    EXPECT_DOUBLE_EQ(out[7], 3.140592653839794);
    EXPECT_DOUBLE_EQ(out[8], 3.141591653589774);
  }

  {
    std::string s{"The length of this string is 42 characters"};
#if PREVIEW_CXX_VERSION >= 17
    auto tv = ranges::transform_view{s, [](char c) -> char {
      return std::toupper(c);
    }};
#else
    auto tv = views::transform(s, [](char c) -> char {
      return std::toupper(c);
    });
#endif
    std::vector<char> out;
    for (auto x : tv) { out.push_back(x); }

    EXPECT_TRUE(ranges::equal(out, "THE LENGTH OF THIS STRING IS 42 CHARACTERS"s));
    EXPECT_EQ(tv.size(), 42);
  }

  auto show = [](const unsigned char) {};

  std::string in{"cppreference.com\n"};
  ranges::for_each(in, show);
  ranges::for_each(in | views::transform(rot13), show);
  EXPECT_TRUE(ranges::equal(in | views::transform(rot13), "pccersrerapr.pbz\n"s));

#if !PREVIEW_ANDROID || defined(PREVIEW_NDK_VERSION_MAJOR) && PREVIEW_NDK_VERSION_MAJOR >= 26
  std::string out;
  ranges::copy(views::transform(in, rot13), std::back_inserter(out));
  EXPECT_EQ(out, "pccersrerapr.pbz\n"s);
  ranges::for_each(out, show);
  ranges::for_each(out | views::transform(rot13), show);
  EXPECT_TRUE(ranges::equal(out | views::transform(rot13), "cppreference.com\n"s));
#endif
}

template<typename F, typename BoundArgs>
struct my_bind_front {
  template<typename... T>
  my_bind_front(F f, T&&... args)
      : f_(std::move(f))
      , front_(std::forward_as_tuple(std::forward<T>(args)...)) {}

  template<typename... Args>
  decltype(auto) operator()(Args&&... args) {
    return call(preview::tuple_index_sequence<BoundArgs>{}, std::forward<Args>(args)...);
  }

  template<std::size_t... I, typename... Args>
  decltype(auto) call(std::index_sequence<I...>, Args&&... args) {
    return f_(std::get<I>(front_)..., std::forward<Args>(args)...);
  }

  F f_;
  BoundArgs front_;
};

TEST(VERSIONED(RangesViews), take_view) {
  using namespace std::literals;
  using namespace preview::literals;

  { // begin 1
    static constexpr auto v = {"Ax"_sv, "By"_sv, "c"_sv, "d"_sv};
#if PREVIEW_CXX_VERSION >= 17
    auto view = ranges::take_view(v, 8);
#else
    auto view = views::take(v, 8);
#endif
    auto iter = view.begin();
    EXPECT_EQ(*iter, "Ax"_sv);

    EXPECT_TRUE_TYPE(ranges::sized_range<decltype(v)>);
    EXPECT_TRUE_TYPE(ranges::random_access_range<decltype(v)>);
    EXPECT_TRUE_TYPE(std::is_same<decltype(iter), decltype(ranges::begin(v))>);
  }
  { // begin 2
    {
      std::forward_list<preview::string_view> v = {"Ax"_sv, "Ey"_sv, "p"_sv, "q"_sv};
#if PREVIEW_CXX_VERSION >= 17
      auto view = ranges::take_view(v, 8);
#else
      auto view = views::take(v, 8);
#endif
      auto iter = view.begin();
      EXPECT_EQ(*iter, "Ax"_sv);

      EXPECT_FALSE_TYPE(ranges::sized_range<decltype(v)>);
      EXPECT_FALSE_TYPE(ranges::random_access_range<decltype(v)>);
      EXPECT_TRUE_TYPE(std::is_same<
          decltype(iter),
          preview::counted_iterator<std::forward_list<preview::string_view>::iterator>
      >);
    }
  }

  { // end
    const auto list1 = {3, 1, 4, 1, 5};
    const auto seq1 = list1 | views::take(4);

    EXPECT_TRUE_TYPE(ranges::sized_range<decltype(seq1)>);
    EXPECT_TRUE_TYPE(ranges::random_access_range<decltype(seq1)>);
    EXPECT_TRUE_TYPE(std::is_same<decltype(seq1.end()), decltype(list1.end())>);

    for (auto it = seq1.begin(); it != seq1.end(); ++it) { (void)*it; }
    EXPECT_TRUE(ranges::equal(seq1, {3, 1, 4, 1}));

    std::list<int> list2 = {2, 7, 1, 8, 2};
    const auto seq2 = list2 | views::take(4);
    EXPECT_TRUE_TYPE (ranges::sized_range<decltype(seq2)>);
    EXPECT_FALSE_TYPE(ranges::random_access_range<decltype(seq2)>);
    EXPECT_TRUE_TYPE (std::is_same<decltype(seq2.end()), preview::default_sentinel_t>);

    for (auto it = seq2.begin(); it != preview::default_sentinel; ++it) { (void)*it; }
    EXPECT_TRUE(ranges::equal(seq2, {2, 7, 1, 8}));
  }

  { // size
    constexpr int arr[]{1, 2, 3};
    EXPECT_TRUE(ranges::equal(
        views::iota(0, 6) | views::transform(preview::bind_front(views::take, arr)),
        {0, 1, 2, 3, 3, 3},
        {},
        [](auto&& tv) { return tv.size(); }
    ));
  }

  constexpr char pi[]{'3', '.', '1', '4', '1', '5', '9', '2'};

  std::string out;
  auto print = [&](char x){ out.push_back(x); };

  ranges::for_each(pi | views::take(6), print);
  EXPECT_EQ(out, "3.1415"s);

  // safely takes 8 chars only, i.e. min(pi.size(), 42)
  out.clear();
#if PREVIEW_CXX_VERSION >= 17
  ranges::for_each(ranges::take_view{pi, 42}, print);
#else
  ranges::for_each(views::take(pi, 42), print);
#endif
  EXPECT_EQ(out, "3.141592");
}

TEST(VERSIONED(RangesViews), take_while_view) {
  using namespace std::literals;
  using namespace preview::literals;

#if PREVIEW_CXX_VERSION >= 17
  for (int year : views::iota(2020)
      | views::take_while([](int y){ return y < 2026; })) { (void)year; }
#endif

  auto r1 = views::iota(10) | views::take_while([](int x) { return true; });
  auto r2 = views::iota(10, 20) | views::take_while([](int x) { return true; });

  auto it1 = r1.end();
  auto it2 = r2.end();

  EXPECT_TRUE(ranges::equal(
      views::iota(2020) | views::take_while([](int y){ return y < 2026; }),
      {2020, 2021, 2022, 2023, 2024, 2025}
  ));

  const char note[]{"Today is yesterday's tomorrow!..."};
#if PREVIEW_CXX_VERSION >= 17
  for (char x : ranges::take_while_view(note, [](char c){ return c != '.'; })) { (void)x; }
#endif

#if PREVIEW_CXX_VERSION >= 17
  EXPECT_TRUE(ranges::equal(
      ranges::take_while_view(note, [](char c){ return c != '.'; }),
      "Today is yesterday's tomorrow!"_sv
  ));
#else
  EXPECT_TRUE(ranges::equal(
      views::take_while(note, [](char c){ return c != '.'; }),
      "Today is yesterday's tomorrow!"_sv
  ));
#endif
}

TEST(VERSIONED(RangesViews), drop_view) {
  using namespace std::literals;
  using namespace preview::literals;

  { // ctor
    constexpr std::array<char, 12> hi{'H', 'e', 'l', 'l', 'o', ',', ' ', 'C', '+', '+', '2', '0'};

    ranges::for_each(hi, [](const char c){ std::cout << c; });
    EXPECT_TRUE(ranges::equal(hi, "Hello, C++20"_sv));

#if PREVIEW_CXX_VERSION >= 20 && (!defined(PREVIEW_NDK_VERSION_MAJOR) || PREVIEW_NDK_VERSION_MAJOR >= 26)
    constexpr
#endif
    auto n = std::distance(hi.cbegin(), ranges::find(hi, 'C'));

#if PREVIEW_CXX_VERSION >= 17
    auto cxx = ranges::drop_view{hi, n};
#else
    auto cxx = views::drop(hi, n);
#endif

    ranges::for_each(cxx, [](const char c){ std::cout << c; });
    EXPECT_EQ(*cxx.begin(), 'C');
    EXPECT_TRUE(ranges::equal(cxx, "C++20"_sv));
  }

  { // end
    constexpr char url[]{"https://cppreference.com"};

    const auto p = std::distance(ranges::begin(url), ranges::find(url, '/'));
#if PREVIEW_CXX_VERSION >= 17
    auto site = ranges::drop_view{url, p + 2}; // drop the prefix "https://"
#else
    auto site = url | views::drop(p + 2);
#endif

    for (auto it = site.begin(); it != site.end(); ++it)
      std::cout << *it; //                ^^^
    std::cout << '\n';

#if PREVIEW_CXX_VERSION >= 17
    EXPECT_TRUE(ranges::equal(ranges::subrange{site.begin(), site.end()}, "cppreference.com"));
#else
    EXPECT_TRUE(ranges::equal(ranges::make_subrange(site.begin(), site.end()), "cppreference.com"));
#endif
  }

  { // size
    constexpr std::array<int, 3> a{42, 43, 44};
#if PREVIEW_CXX_VERSION >= 17
    static_assert(ranges::drop_view{views::all(a), 0}.size() == 3);
    static_assert(ranges::drop_view{views::all(a), 1}.size() == 2);
    static_assert(ranges::drop_view{views::all(a), 2}.size() == 1);
    static_assert(ranges::drop_view{views::all(a), 3}.size() == 0);
    static_assert(ranges::drop_view{views::all(a), 4}.size() == 0);
#else
    EXPECT_EQ(views::drop(views::all(a), 0).size(), 3);
    EXPECT_EQ(views::drop(views::all(a), 1).size(), 2);
    EXPECT_EQ(views::drop(views::all(a), 2).size(), 1);
    EXPECT_EQ(views::drop(views::all(a), 3).size(), 0);
    EXPECT_EQ(views::drop(views::all(a), 4).size(), 0);
#endif
  }

  auto ints = views::iota(0) | views::take(10);
#if PREVIEW_CXX_VERSION >= 17
  for (auto i : ints | views::drop(5)) { (void)i; }
#endif
  EXPECT_TRUE(ranges::equal(
      ints | views::drop(5),
      {5, 6, 7, 8, 9}
  ));

  const auto nums = {1, 2, 3, 4, 5, 6, 7};
  std::vector<int> empty_nums;

#if PREVIEW_CXX_VERSION >= 17
  EXPECT_TRUE(ranges::equal(ranges::drop_view{nums, 2}, {3, 4, 5, 6, 7}));
#else
  EXPECT_TRUE(ranges::equal(views::drop(nums, 2), {3, 4, 5, 6, 7}));
#endif
  EXPECT_TRUE(ranges::equal(nums | views::drop(3), {4, 5, 6, 7}));
  EXPECT_TRUE(ranges::equal(views::iota(1, 8) | views::drop(4), {5, 6, 7}));
  EXPECT_TRUE(ranges::equal(views::iota(1, 8) | views::drop(5), {6, 7}));
  EXPECT_TRUE(ranges::equal(views::iota(1, 8) | views::drop(6), {7}));
  EXPECT_TRUE(ranges::equal(views::iota(1, 8) | views::drop(7), empty_nums));
  EXPECT_TRUE(ranges::equal(views::iota(1, 8) | views::drop(890), empty_nums));
  EXPECT_TRUE(ranges::equal(views::iota(1, 8) | views::drop(100500), empty_nums));
}

bool is_space(char q) noexcept {
  static auto ws = {' ', '\t', '\n', '\v', '\r', '\f'};
  return ranges::any_of(ws, [q](auto p) { return p == q; });
};

std::string trim_left(preview::string_view const in) noexcept {
  auto view = in | views::drop_while(is_space);
  return {view.begin(), view.end()};
}

std::string trim(preview::string_view const in) {
  auto view = in
      | views::drop_while(is_space) | views::reverse
      | views::drop_while(is_space) | views::reverse;
  return {view.begin(), view.end()};
}

TEST(VERSIONED(RangesViews), drop_while_view) {
  using namespace std::literals;
  using namespace preview::literals;

  { // ctor
    constexpr std::array<int, 8> data{0, -1, -2, 3, 1, 4, 1, 5};

#if PREVIEW_CXX_VERSION >= 17
    auto view = ranges::drop_while_view{data, [](int x) { return x <= 0; }};
#else
    auto view = views::drop_while(data, [](int x) { return x <= 0; });
#endif

    for (int x : view)
      std::cout << x << ' ';
    std::cout << '\n';
    EXPECT_TRUE(ranges::equal(view, {3, 1, 4, 1, 5}));
    EXPECT_TRUE(ranges::equal(data, views::concat(views::repeat(true, 3), views::repeat(false, 5)), {}, view.pred()));
    EXPECT_EQ(*view.begin(), 3);

    for (auto it = view.begin(); it != view.end(); ++it)
      std::cout << *it << ' ';
    std::cout << '\n';
  }

  { // base
    std::array<int, 5> data{1, 2, 3, 4, 5};
    auto func = [](int x) { return x < 3; };
#if PREVIEW_CXX_VERSION >= 17
    auto view = ranges::drop_while_view{data, func};
#else
    auto view = views::drop_while(data, func);
#endif
    EXPECT_TRUE(ranges::equal(view, {3, 4, 5}));

    auto base = view.base(); // `base` refers to the `data`
    (void)base;
  }

  EXPECT_EQ(trim_left(" \n C++23"), "C++23"_sv);
  preview::string_view src = " \f\n\t\r\vHello, C++20!\f\n\t\r\v ";

  const auto s = trim(src);
  EXPECT_EQ(s, "Hello, C++20!"s);

  static constexpr auto v = {0, 1, 2, 3, 4, 5};
  for (int n : v | views::drop_while([](int i) { return i < 3; }))
    std::cout << n << ' ';
  std::cout << '\n';
  EXPECT_TRUE(ranges::equal(
      v | views::drop_while([](int i) { return i < 3; }),
      {3, 4, 5}
  ));
}
