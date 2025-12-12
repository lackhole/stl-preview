#include "preview/numeric.h"
#include "test_utils.h"

#include <list>
#include <random>
#include <vector>

#include "preview/algorithm.h"
#include "preview/functional.h"
#include "preview/ranges.h"

namespace ranges = preview::ranges;
namespace views = preview::views;

TEST(VERSIONED(Numeric), iota) {
  std::list<int> list(8);

  // Fill the list with ascending values: 0, 1, 2, ..., 7
  ranges::iota(list, 0);
  EXPECT_TRUE(ranges::equal(list, views::iota(0, 8)));

  // A vector of iterators (see the comment to Example)
  std::vector<std::list<int>::iterator> vec(list.size());

  // Fill with iterators to consecutive list's elements
  ranges::iota(vec.begin(), vec.end(), list.begin());
  EXPECT_TRUE(ranges::equal(
      vec, views::iota(0) | views::take(vec.size()), {},
      ranges::indirect{}
  ));
}

TEST(VERSIONED(Numeric), gcd) {
  constexpr int p{2 * 2 * 3};
  constexpr int q{2 * 3 * 3};
  static_assert(2 * 3 == preview::gcd(p, q), "");

  static_assert(preview::gcd( 6,  10) == 2, "");
  static_assert(preview::gcd( 6, -10) == 2, "");
  static_assert(preview::gcd(-6, -10) == 2, "");

  static_assert(preview::gcd( 24, 0) == 24, "");
  static_assert(preview::gcd(-24, 0) == 24, "");
}

TEST(VERSIONED(Numeric), lcm) {
  constexpr int p{2 * 2 * 3};
  constexpr int q{2 * 3 * 3};
  static_assert(2 * 2 * 3 * 3 == preview::lcm(p, q), "");
  static_assert(225 == preview::lcm(45, 75), "");

  static_assert(preview::lcm( 6,  10) == 30, "");
  static_assert(preview::lcm( 6, -10) == 30, "");
  static_assert(preview::lcm(-6, -10) == 30, "");

  static_assert(preview::lcm( 24, 0) == 0, "");
  static_assert(preview::lcm(-24, 0) == 0, "");

  static_assert(preview::lcm(preview::lcm(2 * 3, 3 * 4), 4 * 5) == 60, "");
  static_assert(preview::lcm(preview::lcm(2 * 3 * 4, 3 * 4 * 5), 4 * 5 * 6) == 120, "");
  static_assert(preview::lcm(preview::lcm(preview::lcm(2 * 3 * 4, 3 * 4 * 5), 4 * 5 * 6), 5 * 6 * 7) == 840, "");
}
