#include "preview/tuple.h"
#include "test_utils.h"

// Custom
TEST(VERSIONED(Tuple), tuple_for_each) {
  {
    std::tuple<> t;
    int sum{};
    preview::tuple_for_each(t, [&sum](const auto& x) { sum += x; });
    EXPECT_EQ(sum, 0);
  }
  {
    std::tuple<int, int> t{1, 2};
    int sum{};
    preview::tuple_for_each(t, [&sum](const auto& x) { sum += x; });
    EXPECT_EQ(sum, 3);
  }
}
