#include "preview/ranges.h"
#include "gtest.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <list>
#include <vector>

#include "preview/algorithm.h"
#include "preview/concepts.h"
#include "preview/span.h"
#include "preview/string_view.h"

TEST(VERSIONED(Ranges), basics) {
  auto const ints = {0, 1, 2, 3, 4, 5};
  auto even = [](int i) { return 0 == i % 2; };
  auto square = [](int i) { return i * i; };

  // the "pipe" syntax of composing the views:
  for (int i : ints | preview::views::filter(even) | preview::views::transform(square))
    std::cout << i << ' ';

  auto cr = ints | preview::views::filter(even) | preview::views::transform(square) | preview::views::common;
  const auto answer = {0, 4, 16};
  EXPECT_TRUE(std::equal(cr.begin(), cr.end(), answer.begin(), answer.end()));

  std::cout << '\n';

  // a traditional "functional" composing syntax:
  for (int i : preview::views::transform(preview::views::filter(ints, even), square))
    std::cout << i << ' ';

  auto t = preview::views::transform(preview::views::filter(ints, even), square);
  EXPECT_TRUE(std::equal(t.begin(), t.end(), answer.begin(), answer.end()));
}

TEST(VERSIONED(Ranges), ranges_begin) {
  std::vector<int> v{3, 1, 4};
  auto vi = preview::ranges::begin(v);
  auto vci = preview::ranges::cbegin(v);
  EXPECT_EQ(*vi, 3);
  EXPECT_EQ(*vi, *vci);

  EXPECT_EQ(*++vi, 1);

  EXPECT_EQ(*++vci, 1); // OK: vci is modifiable object

  *vi = 42; // OK: vi points to mutable element
  EXPECT_EQ(*vi, 42);
  EXPECT_EQ(v[1], 42);
  EXPECT_FALSE((std::is_assignable<decltype(*vci), int>::value));
  // *vci = 13; // Error: vci points to immutable element

  int a[]{-5, 10, 15};
  auto ai = preview::ranges::begin(a); // works with C-arrays as well
  EXPECT_EQ(*ai, -5);
  *ai = 42; // OK
  EXPECT_EQ(*ai, 42);
  EXPECT_EQ(a[0], 42);
}

TEST(VERSIONED(Ranges), ranges_end) {
  std::vector<int> vec{3, 1, 4};
  EXPECT_TRUE (preview::ranges::find(vec, 5) == preview::ranges::end(vec));

  int arr[]{5, 10, 15};
  EXPECT_TRUE (preview::ranges::find(arr, 5) != preview::ranges::end(arr));
}

TEST(VERSIONED(Ranges), ranges_cbegin) {
  std::vector<int> v{3, 1, 4};
  auto vi = preview::ranges::cbegin(v);
  EXPECT_EQ(3, *vi);
  ++vi; // OK, constant-iterator object is mutable
  EXPECT_EQ(1, *vi);
  EXPECT_FALSE((std::is_assignable<decltype(*vi), int>::value));

  int a[]{3, 1, 4};
  auto ai = preview::ranges::cbegin(a); // cbegin works with C-arrays as well
  EXPECT_EQ(3, *ai);
  EXPECT_EQ(*(ai + 1), 1);
  EXPECT_FALSE((std::is_assignable<decltype(*ai), int>::value));
  // *ai = 13; // Error: read-only variable is not assignable
}

TEST(VERSIONED(Ranges), ranges_cend) {
  std::vector<int> vec{3, 1, 4};
  int arr[]{5, 10, 15};

  EXPECT_EQ(preview::ranges::find(vec, 5), preview::ranges::cend(vec));
  EXPECT_NE(preview::ranges::find(arr, 5), preview::ranges::cend(arr));
}

TEST(VERSIONED(Ranges), ranges_rbegin) {
  std::vector<int> v = {3, 1, 4};
  auto vi = preview::ranges::rbegin(v);
  EXPECT_EQ(*vi, 4);
  *vi = 42; // OK
  EXPECT_EQ(*vi, 42);
  EXPECT_EQ(v.back(), 42);

  int a[] = {-5, 10, 15};
  auto ai = preview::ranges::rbegin(a);
  EXPECT_EQ(*ai, 15);
  *ai = 42; // OK
  EXPECT_EQ(*ai, 42);
  EXPECT_EQ(a[2], 42);

  EXPECT_FALSE ((preview::is_invocable<decltype(preview::ranges::rbegin), std::vector<int>>::value));
  // ill-formed: the argument is a rvalue

  auto si = preview::ranges::rbegin(preview::span<int, 3>{a}); // OK
  static_assert(preview::ranges::enable_borrowed_range<
      std::remove_cv_t<decltype(preview::span<int, 3>{a})>>::value, "");
  *si = 43; // OK
  EXPECT_EQ(*si, 43);
  EXPECT_EQ(a[2], 43);
}

TEST(VERSIONED(Ranges), ranges_rend) {
  std::vector<int> v = {3, 1, 4};
  namespace ranges = preview::ranges;
  EXPECT_TRUE (ranges::find(ranges::rbegin(v), ranges::rend(v), 5) == ranges::rend(v));

  int a[] = {5, 10, 15};
  EXPECT_TRUE (ranges::find(ranges::rbegin(a), ranges::rend(a), 5) != ranges::rend(a));
}

TEST(VERSIONED(Ranges), ranges_crbegin) {
  std::vector<int> v{3, 1, 4};
  auto vi = preview::ranges::crbegin(v);
  EXPECT_EQ(*vi, 4);
  ++vi; // OK, iterator object is mutable
  EXPECT_EQ(*vi, 1);
  EXPECT_FALSE((std::is_assignable<decltype(*vi), int>::value));
  // *vi = 13; // Error: underlying element is read-only

  int a[]{-5, 10, 15};
  auto ai = preview::ranges::crbegin(a);
  EXPECT_EQ(*ai, 15);

  // auto x_x = preview::ranges::crbegin(std::vector<int>{6, 6, 6});
  // ill-formed: the argument is a rvalue (see Notes ↑)
  EXPECT_FALSE((preview::is_invocable<decltype(preview::ranges::crbegin), std::vector<int>>::value));

  auto si = preview::ranges::crbegin(preview::span<int, 3>{a}); // OK
  EXPECT_EQ(*si, 15);
  static_assert(
      preview::ranges::enable_borrowed_range<
          std::remove_cv_t<decltype(preview::span<int, 3>{a})>
      >::value,
      "");
}

TEST(VERSIONED(Ranges), ranges_crend) {
  int a[]{4, 6, -3, 9, 10};
  const int b[]{10, 9, -3, 6, 4};

  namespace ranges = preview::ranges;
  EXPECT_TRUE(preview::ranges::equal(
      ranges::rbegin(a), ranges::rend(a),
      ranges::begin(b), ranges::end(b)
  ));

  std::cout << "Vector backwards: ";
  std::vector<int> v{4, 6, -3, 9, 10};
  EXPECT_TRUE(std::equal(ranges::rbegin(v), ranges::rend(v), b, b + 5));
}

TEST(VERSIONED(Ranges), ranges_size) {
  auto v = std::vector<int>{};
  EXPECT_EQ(preview::ranges::size(v), 0);

  auto il = {7};     // std::initializer_list
  EXPECT_EQ(preview::ranges::size(il), 1);

  int array[]{4, 5}; // array has a known bound
  EXPECT_EQ(preview::ranges::size(array), 2);

  EXPECT_FALSE(std::is_signed<decltype(preview::ranges::size(v))>::value);
}

TEST(VERSIONED(Ranges), ranges_ssize) {
  std::array<int, 5> arr{1, 2, 3, 4, 5};
  auto s = preview::ranges::ssize(arr);
  EXPECT_EQ(s, 5);
  EXPECT_TRUE(std::is_signed<decltype(s)>::value);

  for (--s; s >= 0; --s) {}
  EXPECT_EQ(s, -1);
}

TEST(VERSIONED(Ranges), ranges_empty) {
  {
    auto v = std::vector<int>{1, 2, 3};
    EXPECT_FALSE(preview::ranges::empty(v));
    v.clear();
    EXPECT_TRUE (preview::ranges::empty(v));
  }
  {
    auto il = {7, 8, 9};
    EXPECT_FALSE(preview::ranges::empty(il));
    EXPECT_TRUE (preview::ranges::empty(std::initializer_list<int>{}));
  }
  {
    int array[] = {4, 5, 6}; // array has a known bound
    EXPECT_FALSE(preview::ranges::empty(array));
  }
  {
    struct Scanty : private std::vector<int>
    {
      using std::vector<int>::begin;
      using std::vector<int>::end;
      using std::vector<int>::push_back;
      // Note: both empty() and size() are hidden
    };

    Scanty y;
    EXPECT_TRUE (preview::ranges::empty(y));
    y.push_back(42);
    EXPECT_FALSE(preview::ranges::empty(y));
  }
}

TEST(VERSIONED(Ranges), ranges_data) {
  std::string s{"Hello world!\n"};

  char a[20]; // storage for a C-style string
  preview::ranges::data(s);
  std::strcpy(a, preview::ranges::data(s));
  // [data(s), data(s) + size(s)] is guaranteed to be an NTBS
  EXPECT_TRUE((std::strncmp(a, s.c_str(), 20) == 0));
}

TEST(VERSIONED(Ranges), ranges_cdata) {
  std::string src {"hello world!\n"};

//  preview::ranges::cdata(src)[0] = 'H'; // error, src.data() is treated as read-only
  EXPECT_FALSE((std::is_assignable<decltype(preview::ranges::cdata(src)[0]), int>::value));

  preview::ranges::data(src)[0] = 'H'; // OK, src.data() is a non-const storage
  EXPECT_EQ(src[0], 'H');

  char dst[20]; // storage for a C-style string
  std::strcpy(dst, preview::ranges::cdata(src));
  EXPECT_TRUE((std::strncmp(dst, src.c_str(), 20) == 0));
  // [data(src), data(src) + size(src)] is guaranteed to be an NTBS
}

TEST(VERSIONED(Ranges), ranges_dangling) {
  auto get_array_by_value = [] {
    return std::array<int, 4>{0, 1, 0, 1};
  };
  auto dangling_iter = preview::ranges::max_element(get_array_by_value());
  static_assert(std::is_same<preview::ranges::dangling, decltype(dangling_iter)>::value, "");
  static_assert(preview::dereferenceable<decltype(dangling_iter)>::value == false, "");
  //  *dangling_iter << '\n'; // compilation error: no match for 'operator*'
  //  (operand type is 'std::ranges::dangling')

  auto get_persistent_array = []() -> const std::array<int, 4>& {
    static constexpr std::array<int, 4> a{0, 1, 0, 1};
    return a;
  };
  auto valid_iter = preview::ranges::max_element(get_persistent_array());
  static_assert(!std::is_same<preview::ranges::dangling, decltype(valid_iter)>::value, "");
  EXPECT_EQ(*valid_iter, 1);

  auto get_string_view = [] { return preview::string_view{"alpha"}; };
  auto valid_iter2 = preview::ranges::min_element(get_string_view());
  // OK: std::basic_string_view models borrowed_range
  static_assert(!std::is_same<preview::ranges::dangling, decltype(valid_iter2)>::value, "");
  EXPECT_EQ(*valid_iter2, 'a');
}

TEST(VERSIONED(Ranges), to) {
  auto map = preview::views::iota('A', 'E') |
      preview::views::enumerate |
      preview::ranges::to<std::map>();

  std::cout << map[0] << std::endl;
  std::cout << map[1] << std::endl;
  std::cout << map[2] << std::endl;
}


TEST(VERSIONED(Ranges), iterator_conforming) {
  auto r = preview::views::iota(0) |
      preview::views::take(10) |
      preview::views::filter([](auto x) { return x % 2 == 0; });

  static_assert(std::is_same<
      std::iterator_traits<decltype(r.begin())>::reference,
      int
  >::value, "");

  std::vector<char> v = {'h', 'e', 'l'};
  std::string s = "lo, ";
  preview::string_view sv = "world";
  std::list<char> l = {'!'};

  for (auto c : preview::views::concat(v, s, sv, l)) {
    std::cout << c;
  }
}
