#include "preview/numeric.h"
#include "gtest.h"

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
