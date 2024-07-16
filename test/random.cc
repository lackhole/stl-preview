#include "preview/random.h"
#include "gtest.h"

#include <random>


TEST(VERSIONED(Random), uniform_random_bit_generator) {
  struct my_random_generator {
    unsigned int operator()() const { return 1998; }
  };

  struct my_random_generator2 : my_random_generator {
    static unsigned int min() { return 0; }
    static unsigned int max() { return 0; }
  };

  struct my_random_generator3 : my_random_generator {
    static constexpr unsigned int min() { return 0; }
    static constexpr unsigned int max() { return 2000; }
  };

  EXPECT_FALSE_TYPE(preview::uniform_random_bit_generator<int>);
  EXPECT_FALSE_TYPE(preview::uniform_random_bit_generator<my_random_generator>);
  EXPECT_FALSE_TYPE(preview::uniform_random_bit_generator<my_random_generator2>);
  EXPECT_TRUE_TYPE(preview::uniform_random_bit_generator<my_random_generator3>);
  EXPECT_TRUE_TYPE(preview::uniform_random_bit_generator<std::mt19937>);
}
