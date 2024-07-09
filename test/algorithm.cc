#include <algorithm>
#include <cstdint>
#include <vector>

#include "preview/algorithm.h"
#include "preview/ranges.h"

#include "gtest.h"

namespace ranges = preview::ranges;
namespace views  = preview::views;
using namespace std::literals;
using namespace preview::literals;

TEST(VERSIONED(Algorithm), clamp) {
  EXPECT_EQ(preview::clamp(-129, INT8_MIN, INT8_MAX), -128);
  EXPECT_EQ(preview::clamp(-128, INT8_MIN, INT8_MAX), -128);
  EXPECT_EQ(preview::clamp(  -1, INT8_MIN, INT8_MAX),   -1);
  EXPECT_EQ(preview::clamp(   0, INT8_MIN, INT8_MAX),   0);
  EXPECT_EQ(preview::clamp(  42, INT8_MIN, INT8_MAX),  42);
  EXPECT_EQ(preview::clamp( 127, INT8_MIN, INT8_MAX), 127);
  EXPECT_EQ(preview::clamp( 128, INT8_MIN, INT8_MAX), 127);
  EXPECT_EQ(preview::clamp( 255, INT8_MIN, INT8_MAX), 127);
  EXPECT_EQ(preview::clamp( 256, INT8_MIN, INT8_MAX), 127);

  EXPECT_EQ(preview::clamp(-129, 0, UINT8_MAX),   0);
  EXPECT_EQ(preview::clamp(-128, 0, UINT8_MAX),   0);
  EXPECT_EQ(preview::clamp(  -1, 0, UINT8_MAX),   0);
  EXPECT_EQ(preview::clamp(   0, 0, UINT8_MAX),   0);
  EXPECT_EQ(preview::clamp(  42, 0, UINT8_MAX),  42);
  EXPECT_EQ(preview::clamp( 127, 0, UINT8_MAX), 127);
  EXPECT_EQ(preview::clamp( 128, 0, UINT8_MAX), 128);
  EXPECT_EQ(preview::clamp( 255, 0, UINT8_MAX), 255);
  EXPECT_EQ(preview::clamp( 256, 0, UINT8_MAX), 255);
}

TEST(VERSIONED(Algorithm), for_each_n) {
  std::vector<int> v1 = {1, 2, 3, 4, 5};
  std::vector<int> v2 = {2, 4, 6, 4, 5};
  preview::for_each_n(v1.begin(), 3, [](auto& x) { x *= 2; });
  EXPECT_TRUE(std::equal(v1.begin(), v1.end(), v2.begin(), v2.end()));
}

struct S {
  int value{0};
  bool specified_state{true};

  S(int v = 0) : value{v} {}
  S(S const& rhs) = default;
  S(S&& rhs) { *this = std::move(rhs); }
  S& operator=(S const& rhs) = default;
  S& operator=(S&& rhs) {
    if (this != &rhs)
    {
      value = rhs.value;
      specified_state = rhs.specified_state;
      rhs.specified_state = false;
      rhs.value = -1;
    }
    return *this;
  }
};
TEST(VERSIONED(Algorithm), shift) {
  {
    std::vector<S> a{1, 2, 3, 4, 5, 6, 7};

    preview::shift_left(begin(a), end(a), 3);
    EXPECT_TRUE(ranges::equal(a, {4, 5, 6, 7, -1, -1, -1}, {}, &S::value));

    preview::shift_right(begin(a), end(a), 2);
    EXPECT_TRUE(ranges::equal(a, {-1, -1, 4, 5, 6, 7, -1}, {}, &S::value));

    preview::shift_left(begin(a), end(a), 8); // has no effect: n >= last - first
    EXPECT_TRUE(ranges::equal(a, {-1, -1, 4, 5, 6, 7, -1}, {}, &S::value));
  }
  {
    std::vector<int> b{1, 2, 3, 4, 5, 6, 7};

    preview::shift_left(begin(b), end(b), 3);
    EXPECT_TRUE(ranges::equal(b, {4, 5, 6, 7, 5, 6, 7}));

    preview::shift_right(begin(b), end(b), 2);
    EXPECT_TRUE(ranges::equal(b, {4, 5, 4, 5, 6, 7, 5}));

    preview::shift_left(begin(b), end(b), 8); // ditto
    EXPECT_TRUE(ranges::equal(b, {4, 5, 4, 5, 6, 7, 5}));
  }
  {
    std::vector<std::string> c{"a", "b", "c", "d", "e", "f", "g"};

    preview::shift_left(begin(c), end(c), 3);
    EXPECT_TRUE(ranges::equal(c, {"d"s, "e"s, "f"s, "g"s, ""s, ""s, ""s}));

    preview::shift_right(begin(c), end(c), 2);
    EXPECT_TRUE(ranges::equal(c, {""s, ""s, "d"s, "e"s, "f"s, "g"s, ""s}));

    preview::shift_left(begin(c), end(c), 8); // ditto
    EXPECT_TRUE(ranges::equal(c, {""s, ""s, "d"s, "e"s, "f"s, "g"s, ""s}));
  }
}
