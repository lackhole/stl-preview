#include "preview/ranges.h"
#include "gtest.h"

#include <algorithm>

// TODO: Write test
TEST(VERSIONED(Ranges), basics) {}

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
