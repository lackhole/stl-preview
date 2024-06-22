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

namespace ranges = preview::ranges;
namespace views = preview::views;

TEST(VERSIONED(Ranges), basics) {
  auto const ints = {0, 1, 2, 3, 4, 5};
  auto even = [](int i) { return 0 == i % 2; };
  auto square = [](int i) { return i * i; };

  // the "pipe" syntax of composing the views:
  for (int i : ints | views::filter(even) | views::transform(square))
    std::cout << i << ' ';

  auto cr = ints | views::filter(even) | views::transform(square) | views::common;
  const auto answer = {0, 4, 16};
  EXPECT_TRUE(std::equal(cr.begin(), cr.end(), answer.begin(), answer.end()));

  std::cout << '\n';

  // a traditional "functional" composing syntax:
  for (int i : views::transform(views::filter(ints, even), square))
    std::cout << i << ' ';

  auto t = views::transform(views::filter(ints, even), square);
  EXPECT_TRUE(std::equal(t.begin(), t.end(), answer.begin(), answer.end()));
}

TEST(VERSIONED(Ranges), ranges_begin) {
  std::vector<int> v{3, 1, 4};
  auto vi = ranges::begin(v);
  auto vci = ranges::cbegin(v);
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
  auto ai = ranges::begin(a); // works with C-arrays as well
  EXPECT_EQ(*ai, -5);
  *ai = 42; // OK
  EXPECT_EQ(*ai, 42);
  EXPECT_EQ(a[0], 42);
}

TEST(VERSIONED(Ranges), ranges_end) {
  std::vector<int> vec{3, 1, 4};
  EXPECT_TRUE (ranges::find(vec, 5) == ranges::end(vec));

  int arr[]{5, 10, 15};
  EXPECT_TRUE (ranges::find(arr, 5) != ranges::end(arr));
}

TEST(VERSIONED(Ranges), ranges_cbegin) {
  std::vector<int> v{3, 1, 4};
  auto vi = ranges::cbegin(v);
  EXPECT_EQ(3, *vi);
  ++vi; // OK, constant-iterator object is mutable
  EXPECT_EQ(1, *vi);
  EXPECT_FALSE((std::is_assignable<decltype(*vi), int>::value));

  int a[]{3, 1, 4};
  auto ai = ranges::cbegin(a); // cbegin works with C-arrays as well
  EXPECT_EQ(3, *ai);
  EXPECT_EQ(*(ai + 1), 1);
  EXPECT_FALSE((std::is_assignable<decltype(*ai), int>::value));
  // *ai = 13; // Error: read-only variable is not assignable
}

TEST(VERSIONED(Ranges), ranges_cend) {
  std::vector<int> vec{3, 1, 4};
  int arr[]{5, 10, 15};

  EXPECT_EQ(ranges::find(vec, 5), ranges::cend(vec));
  EXPECT_NE(ranges::find(arr, 5), ranges::cend(arr));
}

TEST(VERSIONED(Ranges), ranges_rbegin) {
  std::vector<int> v = {3, 1, 4};
  auto vi = ranges::rbegin(v);
  EXPECT_EQ(*vi, 4);
  *vi = 42; // OK
  EXPECT_EQ(*vi, 42);
  EXPECT_EQ(v.back(), 42);

  int a[] = {-5, 10, 15};
  auto ai = ranges::rbegin(a);
  EXPECT_EQ(*ai, 15);
  *ai = 42; // OK
  EXPECT_EQ(*ai, 42);
  EXPECT_EQ(a[2], 42);

  EXPECT_FALSE ((preview::is_invocable<decltype(ranges::rbegin), std::vector<int>>::value));
  // ill-formed: the argument is a rvalue

  auto si = ranges::rbegin(preview::span<int, 3>{a}); // OK
  static_assert(ranges::enable_borrowed_range<
      std::remove_cv_t<decltype(preview::span<int, 3>{a})>>, "");
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
  auto vi = ranges::crbegin(v);
  EXPECT_EQ(*vi, 4);
  ++vi; // OK, iterator object is mutable
  EXPECT_EQ(*vi, 1);
  EXPECT_FALSE((std::is_assignable<decltype(*vi), int>::value));
  // *vi = 13; // Error: underlying element is read-only

  int a[]{-5, 10, 15};
  auto ai = ranges::crbegin(a);
  EXPECT_EQ(*ai, 15);

  // auto x_x = ranges::crbegin(std::vector<int>{6, 6, 6});
  // ill-formed: the argument is a rvalue
  EXPECT_FALSE((preview::is_invocable<decltype(ranges::crbegin), std::vector<int>>::value));

  auto si = ranges::crbegin(preview::span<int, 3>{a}); // OK
  EXPECT_EQ(*si, 15);
  static_assert(
      ranges::enable_borrowed_range<
          std::remove_cv_t<decltype(preview::span<int, 3>{a})>
      >,
      "");
}

TEST(VERSIONED(Ranges), ranges_crend) {
  int a[]{4, 6, -3, 9, 10};
  const int b[]{10, 9, -3, 6, 4};

  namespace ranges = preview::ranges;
  EXPECT_TRUE(ranges::equal(
      ranges::rbegin(a), ranges::rend(a),
      ranges::begin(b), ranges::end(b)
  ));

  std::cout << "Vector backwards: ";
  std::vector<int> v{4, 6, -3, 9, 10};
  EXPECT_TRUE(std::equal(ranges::rbegin(v), ranges::rend(v), b, b + 5));
}

TEST(VERSIONED(Ranges), ranges_size) {
  auto v = std::vector<int>{};
  EXPECT_EQ(ranges::size(v), 0);

  auto il = {7};     // std::initializer_list
  EXPECT_EQ(ranges::size(il), 1);

  int array[]{4, 5}; // array has a known bound
  EXPECT_EQ(ranges::size(array), 2);

  EXPECT_FALSE(std::is_signed<decltype(ranges::size(v))>::value);
}

TEST(VERSIONED(Ranges), ranges_ssize) {
  std::array<int, 5> arr{1, 2, 3, 4, 5};
  auto s = ranges::ssize(arr);
  EXPECT_EQ(s, 5);
  EXPECT_TRUE(std::is_signed<decltype(s)>::value);

  for (--s; s >= 0; --s) {}
  EXPECT_EQ(s, -1);
}

TEST(VERSIONED(Ranges), ranges_empty) {
  {
    auto v = std::vector<int>{1, 2, 3};
    EXPECT_FALSE(ranges::empty(v));
    v.clear();
    EXPECT_TRUE (ranges::empty(v));
  }
  {
    auto il = {7, 8, 9};
    EXPECT_FALSE(ranges::empty(il));
    EXPECT_TRUE (ranges::empty(std::initializer_list<int>{}));
  }
  {
    int array[] = {4, 5, 6}; // array has a known bound
    EXPECT_FALSE(ranges::empty(array));
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
    EXPECT_TRUE (ranges::empty(y));
    y.push_back(42);
    EXPECT_FALSE(ranges::empty(y));
  }
}

TEST(VERSIONED(Ranges), ranges_data) {
  std::string s{"Hello world!\n"};

  char a[20]; // storage for a C-style string
  ranges::data(s);
  std::strcpy(a, ranges::data(s));
  // [data(s), data(s) + size(s)] is guaranteed to be an NTBS
  EXPECT_TRUE((std::strncmp(a, s.c_str(), 20) == 0));
}

TEST(VERSIONED(Ranges), ranges_cdata) {
  std::string src {"hello world!\n"};

//  ranges::cdata(src)[0] = 'H'; // error, src.data() is treated as read-only
  EXPECT_FALSE((std::is_assignable<decltype(ranges::cdata(src)[0]), int>::value));

  ranges::data(src)[0] = 'H'; // OK, src.data() is a non-const storage
  EXPECT_EQ(src[0], 'H');

  char dst[20]; // storage for a C-style string
  std::strcpy(dst, ranges::cdata(src));
  EXPECT_TRUE((std::strncmp(dst, src.c_str(), 20) == 0));
  // [data(src), data(src) + size(src)] is guaranteed to be an NTBS
}

TEST(VERSIONED(Ranges), ranges_dangling) {
  auto get_array_by_value = [] {
    return std::array<int, 4>{0, 1, 0, 1};
  };
  auto dangling_iter = ranges::max_element(get_array_by_value());
  static_assert(std::is_same<ranges::dangling, decltype(dangling_iter)>::value, "");
  static_assert(preview::dereferenceable<decltype(dangling_iter)>::value == false, "");
  //  *dangling_iter << '\n'; // compilation error: no match for 'operator*'
  //  (operand type is 'ranges::dangling')

  auto get_persistent_array = []() -> const std::array<int, 4>& {
    static constexpr std::array<int, 4> a{0, 1, 0, 1};
    return a;
  };
  auto valid_iter = ranges::max_element(get_persistent_array());
  static_assert(!std::is_same<ranges::dangling, decltype(valid_iter)>::value, "");
  EXPECT_EQ(*valid_iter, 1);

  auto get_string_view = [] { return preview::string_view{"alpha"}; };
  auto valid_iter2 = ranges::min_element(get_string_view());
  // OK: std::basic_string_view models borrowed_range
  static_assert(!std::is_same<ranges::dangling, decltype(valid_iter2)>::value, "");
  EXPECT_EQ(*valid_iter2, 'a');
}

TEST(VERSIONED(Ranges), to) {
  auto map = views::iota('A', 'E') |
      views::enumerate |
      ranges::to<std::map>();

  std::cout << map[0] << std::endl;
  std::cout << map[1] << std::endl;
  std::cout << map[2] << std::endl;
}

template<class T, class A>
class VectorView : public ranges::view_interface<VectorView<T, A>>
{
 public:
  VectorView() = default;

  VectorView(const std::vector<T, A>& vec) :
      m_begin(vec.cbegin()), m_end(vec.cend())
  {}

  auto begin() const { return m_begin; }

  auto end() const { return m_end; }

 private:
  typename std::vector<T, A>::const_iterator m_begin{}, m_end{};
};

TEST(VERSIONED(Ranges), view_interface) {
  { // empty
    constexpr std::array<int, 5> a{0, 1, 2, 3, 4};
#if PREVIEW_CXX_VERSION >= 17
    static_assert(!ranges::single_view(a).empty());
#else
    static_assert(!ranges::single_view<std::array<int, 5>>(a).empty(), "");
#endif
    EXPECT_TRUE((a | views::take(0)).empty());
    EXPECT_FALSE((a | views::take(5)).empty());
    EXPECT_TRUE((a | views::drop(5)).empty());
    EXPECT_FALSE((a | views::drop(3)).empty());
    static_assert(views::iota(0, 0).empty(), "");
    static_assert(!views::iota(0).empty(), "");
  }

  { // operator bool

    const std::array<int, 5> ints {0, 1, 2, 3, 4};
    auto odds = ints | views::filter([](int i) { return 0 != i % 2; });
    auto negs = ints | views::filter([](int i) { return i < 0; });
    EXPECT_TRUE(!!odds); // Has odd numbers
    EXPECT_FALSE(!!negs); // Doesn't have negative numbers
  }

  { // data
    PREVIEW_CONSTEXPR_AFTER_CXX17 preview::string_view str { "Hello, C++20!" };
    std::cout << (str | views::drop(7)).data() << '\n';
    constexpr static std::array<int, 5> a { 1,2,3,4,5 };
    PREVIEW_CONSTEXPR_AFTER_CXX17 auto v { a | views::take(3) };
#if PREVIEW_CXX_VERSION >= 17
    static_assert( &a[0] == v.data());
#else
    EXPECT_EQ(&a[0], v.data());
#endif
  }

  std::vector<int> v = {1, 4, 9, 16};

  VectorView<int, typename std::vector<int>::allocator_type> view_over_v{v};

  // We can iterate with begin() and end().
  for (int n : view_over_v)
    std::cout << n << ' ';
  std::cout << '\n';

  // We get operator[] for free when inheriting from view_interface
  // since we satisfy the random_access_range concept.
  for (std::ptrdiff_t i = 0; i != view_over_v.size(); ++i)
    std::cout << "v[" << i << "] = " << view_over_v[i] << '\n';
}

template<class V>
void mutate(V& v)
{
  v += 'A' - 'a';
}

template<class K, class V>
void mutate_map_values(std::multimap<K, V>& m, K k)
{
#if PREVIEW_CXX_VERSION >= 20
  auto [first, last] = m.equal_range(k);
  for (auto& [_, v] : ranges::subrange(first, last))
    mutate(v);
#elif PREVIEW_CXX_VERSION >= 17
  auto [first, last] = m.equal_range(k);
  for (auto& [_, v] : ranges::make_subrange(first, last))
    mutate(v);
#else
  auto p =  m.equal_range(k);
  for (auto& p : ranges::make_subrange(p.first, p.second))
    mutate(p.second);
#endif
}

TEST(VERSIONED(Ranges), subrange) {
  std::multimap<int, char> mm{{4, 'a'}, {3, '-'}, {4, 'b'}, {5, '-'}, {4, 'c'}};
  mutate_map_values(mm, 4);

  auto it = mm.begin();
  EXPECT_EQ(it->second, '-'); ++it;
  EXPECT_EQ(it->second, 'A'); ++it;
  EXPECT_EQ(it->second, 'B'); ++it;
  EXPECT_EQ(it->second, 'C'); ++it;
  EXPECT_EQ(it->second, '-'); ++it;
}

// Define Slice as a range adaptor closure
struct Slice : ranges::range_adaptor_closure<Slice> {
  std::size_t start = 0;
  std::size_t end = preview::string_view::npos;

  constexpr preview::string_view operator()(preview::string_view sv) const
  {
    return sv.substr(start, end - start);
  }
};
TEST(VERSIONED(Ranges), range_adaptor_closure) {

  const preview::string_view str = "01234567";

#if PREVIEW_CXX_VERSION >= 20
  Slice slicer{.start = 1, .end = 6};
#else
  Slice slicer{};
  slicer.start = 1;
  slicer.end = 6;
#endif

  auto sv1 = slicer(str); // use slicer as a normal function object
  auto sv2 = str | slicer; // use slicer as a range adaptor closure object
  EXPECT_EQ(sv1, "12345");
  EXPECT_EQ(sv2, "12345");

  // range adaptor closures can be composed
  auto slice_and_drop
      = slicer
          | views::drop_while([](char ch) { return ch != '3'; });
  for (auto&& c : str | slice_and_drop) {
     std::cout << c;
  }

  EXPECT_TRUE((preview::string_view(str | slice_and_drop) == "345"));
}


TEST(VERSIONED(Ranges), iterator_conforming) {
  auto r = views::iota(0) |
      views::take(10) |
      views::filter([](auto x) { return x % 2 == 0; });

  static_assert(std::is_same<
      std::iterator_traits<decltype(r.begin())>::reference,
      int
  >::value, "");

  std::vector<char> v = {'h', 'e', 'l'};
  std::string s = "lo, ";
  preview::string_view sv = "world";
  std::list<char> l = {'!'};

  for (auto c : views::concat(v, s, sv, l)) {
    std::cout << c;
  }
}
