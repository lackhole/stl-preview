#include "preview/bit.h"
#include "gtest.h"

#include <cstdint>

union U {
  int i;
  char c[sizeof(int)];
};

TEST(VERSIONED(bit), bit_cast) {
  U u{10};
  EXPECT_EQ(preview::bit_cast<int>(u.c), 10);

  constexpr double f64v = 19880124.0;
  auto u64v = preview::bit_cast<std::uint64_t>(f64v);
  EXPECT_EQ(preview::bit_cast<double>(u64v), f64v); // round-trip

  constexpr std::uint64_t u64v2 = 0x3fe9000000000000ull;
  auto f64v2 = preview::bit_cast<double>(u64v2);
  EXPECT_EQ(preview::bit_cast<std::uint64_t>(f64v2), u64v2); // round-trip
}
