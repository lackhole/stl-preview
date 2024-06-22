#include "preview/ranges.h"
#include "gtest.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
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

TEST(VERSIONED(RangesConcepts), range) {
  // A minimum range
  struct SimpleRange
  {
    int* begin();
    int* end();
  };
  static_assert(ranges::range<SimpleRange>::value, "");

  // not a range: no begin/end
  struct NotRange
  {
    int t {};
  };
  static_assert(!ranges::range<NotRange>::value, "");

  // not a range: begin does not return an input_or_output_iterator
  struct NotRange2
  {
    void* begin();
    int* end();
  };
  static_assert(!ranges::range<NotRange2>::value, "");
}


template<typename T, std::size_t N>
struct MyRange : std::array<T, N> {};

template<typename T, std::size_t N>
PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(MyRange<T, N>) = false;

template<typename T, std::size_t N>
struct MyBorrowedRange : preview::span<T, N> {};

template<typename T, std::size_t N>
PREVIEW_INLINE_VARIABLE constexpr bool ranges::enable_borrowed_range<MyBorrowedRange<T, N>> = true;

TEST(VERSIONED(RangesConcepts), borrowed_range) {
  EXPECT_TRUE_TYPE  (ranges::range<MyRange<int, 8>>);
  EXPECT_FALSE_TYPE (ranges::borrowed_range<MyRange<int, 8>>);
  EXPECT_TRUE_TYPE  (ranges::range<MyBorrowedRange<int, 8>>);
  EXPECT_TRUE_TYPE  (ranges::borrowed_range<MyBorrowedRange<int, 8>>);

#if PREVIEW_CXX_VERSION >= 17
  auto getMyRangeByValue = []{ return MyRange<int, 4>{{1, 2, 42, 3}}; };
  auto dangling_iter = ranges::max_element(getMyRangeByValue());
  EXPECT_TRUE_TYPE  (std::is_same<ranges::dangling, decltype(dangling_iter)>);
  EXPECT_FALSE_TYPE (preview::dereferenceable<decltype(dangling_iter)>);
//  *dangling_iter; // compilation error (i.e. dangling protection works.)

  auto my = MyRange<int, 4>{{1, 2, 42, 3}};
  auto valid_iter = ranges::max_element(my);
  EXPECT_EQ(*valid_iter, 42);

  auto getMyBorrowedRangeByValue = []
  {
    static int sa[4]{1, 2, 42, 3};
    return MyBorrowedRange<int, preview::size(sa)>{sa};
  };
  auto valid_iter2 = ranges::max_element(getMyBorrowedRangeByValue());
  EXPECT_EQ(*valid_iter2, 42);
#endif
}

TEST(VERSIONED(RangesConcepts), sized_range) {
  EXPECT_TRUE_TYPE(ranges::sized_range<int[4]>);
  EXPECT_TRUE_TYPE(ranges::sized_range<std::array<int, 4>>);
  EXPECT_TRUE_TYPE(ranges::sized_range<std::deque<int>>);
  EXPECT_TRUE_TYPE(ranges::sized_range<std::list<int>>);
  EXPECT_TRUE_TYPE(ranges::sized_range<std::map<int, int>>);
  EXPECT_TRUE_TYPE(ranges::sized_range<std::set<int, int>>);
  EXPECT_TRUE_TYPE(ranges::sized_range<std::unordered_map<int, int>>);
  EXPECT_TRUE_TYPE(ranges::sized_range<std::unordered_set<int, int>>);
  EXPECT_TRUE_TYPE(ranges::sized_range<std::vector<int>>);

  EXPECT_FALSE_TYPE(ranges::sized_range<int[]>);
  EXPECT_FALSE_TYPE(ranges::sized_range<std::forward_list<int>>);
  EXPECT_FALSE_TYPE(ranges::sized_range<std::queue<int>>);
}


struct ArchetypalView : ranges::view_interface<ArchetypalView>
{
  int* begin();
  int* end();
};
TEST(VERSIONED(RangesConcepts), view) {
  EXPECT_TRUE_TYPE(ranges::view<ArchetypalView>);
  EXPECT_TRUE_TYPE(ranges::view<ranges::subrange<typename std::vector<int>::iterator>>);
  EXPECT_TRUE_TYPE(ranges::view<ranges::iota_view<int>>);
  EXPECT_TRUE_TYPE(ranges::view<ranges::iota_view<int, int>>);

  EXPECT_FALSE_TYPE(ranges::view<std::vector<int>>);
}

// TODO: Write test
TEST(VERSIONED(RangesConcepts), input_range) {}

// TODO: Write test
TEST(VERSIONED(RangesConcepts), output_range) {}

TEST(VERSIONED(RangesConcepts), forward_range) {
  const char* str{"not a forward range"};
  const char str2[] = "a forward range";

  EXPECT_TRUE_TYPE(ranges::forward_range<decltype("a forward range")>);
  EXPECT_TRUE_TYPE(ranges::forward_range<decltype(str2)>);
  EXPECT_TRUE_TYPE(ranges::forward_range<std::forward_list<char>>);
  EXPECT_TRUE_TYPE(ranges::forward_range<preview::span<char>>);

  EXPECT_FALSE_TYPE(ranges::forward_range<decltype(str)>);
  EXPECT_FALSE_TYPE(ranges::forward_range<std::stack<char>>);
  EXPECT_FALSE_TYPE(ranges::forward_range<std::tuple<std::forward_list<char>>>);
  EXPECT_FALSE_TYPE(ranges::forward_range<std::queue<char>>);
}

TEST(VERSIONED(RangesConcepts), bidirectional_range) {
  EXPECT_TRUE_TYPE(preview::ranges::bidirectional_range<std::set<int>>);
  EXPECT_TRUE_TYPE(preview::ranges::bidirectional_range<std::list<int>>);
  EXPECT_FALSE_TYPE(preview::ranges::bidirectional_range<std::unordered_set<int>>);
  EXPECT_FALSE_TYPE(preview::ranges::bidirectional_range<std::forward_list<int>>);
}

TEST(VERSIONED(RangesConcepts), random_access_range) {
  int a[4];
  EXPECT_TRUE_TYPE(ranges::random_access_range<std::vector<int>>);
  EXPECT_TRUE_TYPE(ranges::random_access_range<std::vector<bool>>);
  EXPECT_TRUE_TYPE(ranges::random_access_range<std::deque<int>>);
  EXPECT_TRUE_TYPE(ranges::random_access_range<std::valarray<int>>);
  EXPECT_TRUE_TYPE(ranges::random_access_range<decltype(a)>);
  EXPECT_TRUE_TYPE(ranges::random_access_range<std::array<std::list<int>, 42>>);

  EXPECT_FALSE_TYPE(ranges::random_access_range<std::list<int>>);
  EXPECT_FALSE_TYPE(ranges::random_access_range<std::set<int>>);
}

TEST(VERSIONED(RangesConcepts), contiguous_range) {
  int a[4];
  EXPECT_TRUE_TYPE(ranges::contiguous_range<std::vector<int>>);
  EXPECT_TRUE_TYPE(ranges::contiguous_range<std::valarray<int>>);
  EXPECT_TRUE_TYPE(ranges::contiguous_range<decltype(a)>);
  EXPECT_TRUE_TYPE(ranges::contiguous_range<std::array<std::list<int>, 42>>);

  EXPECT_FALSE_TYPE(ranges::contiguous_range<std::vector<bool>>);
  EXPECT_FALSE_TYPE(ranges::contiguous_range<std::deque<int>>);
  EXPECT_FALSE_TYPE(ranges::contiguous_range<std::list<int>>);
  EXPECT_FALSE_TYPE(ranges::contiguous_range<std::set<int>>);
}

TEST(VERSIONED(RangesConcepts), common_range) {
  struct A {
    char* begin();
    char* end();
  };
  EXPECT_TRUE_TYPE(ranges::common_range<A>);

  struct B {
    char* begin();
    bool end();
  };  // not a common_range: begin/end have different types
  EXPECT_FALSE_TYPE(ranges::common_range<B>);

  struct C {
    char* begin();
  };  // not a common_range, not even a range: has no end()
  EXPECT_FALSE_TYPE(ranges::common_range<C>);
}

template<typename T>
auto test_viewable_range(T &&) -> ranges::viewable_range<T&&>;

TEST(VERSIONED(RangesConcepts), viewable_range) {
  auto il = {1, 2, 3};
  int arr []{1, 2, 3};
  std::vector<int> vec{1, 2, 3};
  ranges::ref_view<int[3]> r{arr};
  ranges::owning_view<std::string> o{std::string("Hello")};

  EXPECT_TRUE_TYPE(decltype(test_viewable_range( il )));
  EXPECT_TRUE_TYPE(decltype(test_viewable_range( arr )));
  EXPECT_TRUE_TYPE(decltype(test_viewable_range( vec )));
  EXPECT_TRUE_TYPE(decltype(test_viewable_range( std::move(vec) )));
  EXPECT_TRUE_TYPE(decltype(test_viewable_range( r )));
  EXPECT_TRUE_TYPE(decltype(test_viewable_range( std::move(r) )));
  EXPECT_TRUE_TYPE(decltype(test_viewable_range( std::move(o) )));
  EXPECT_TRUE_TYPE(decltype(test_viewable_range( preview::ranges::ref_view<ranges::owning_view<std::string>>(o) )));

  EXPECT_FALSE_TYPE(decltype(test_viewable_range( std::move(il) )));
  EXPECT_FALSE_TYPE(decltype(test_viewable_range( std::move(arr) )));
  EXPECT_FALSE_TYPE(decltype(test_viewable_range( o )));
}

TEST(VERSIONED(RangesConcepts), constant_range) {
  EXPECT_TRUE_TYPE(ranges::constant_range<const std::vector<int>>);
  EXPECT_TRUE_TYPE(ranges::constant_range<std::string_view>);
  EXPECT_TRUE_TYPE(ranges::constant_range<std::span<const int>>);

  EXPECT_FALSE_TYPE(ranges::constant_range<std::vector<int>>);
  EXPECT_FALSE_TYPE(ranges::constant_range<std::span<int>>);
  EXPECT_FALSE_TYPE(ranges::constant_range<const std::span<int>>);
}
