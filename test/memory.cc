#include "preview/memory.h"
#include "gtest.h"

class S
{
  int x_;
  float y_;
  double z_;
 public:
  constexpr S(int x, float y, double z) : x_{x}, y_{y}, z_{z} {}

  constexpr bool operator==(const S& other) const noexcept {
    return
        x_ == other.x_ &&
        y_ == other.y_ &&
        z_ == other.z_;
  }
};

TEST(VERSIONED(Memory), construct_at) {



  alignas(S) unsigned char storage[sizeof(S)]{};
  S uninitialized = std::bit_cast<S>(storage);
  std::destroy_at(&uninitialized);
  S* ptr = std::construct_at(std::addressof(uninitialized), 42, 2.71f, 3.14);
  const bool res{*ptr == S{42, 2.71f, 3.14}};
  std::destroy_at(ptr);
}
