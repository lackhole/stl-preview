#include "preview/memory.h"
#include "gtest.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "preview/algorithm.h"
#include "preview/bit.h"
#include "preview/ranges.h"

namespace ranges = preview::ranges;
namespace views = preview::views;

struct bizarre_object {
  int operator&() const {
    return 3;
  }
};

constexpr int global_y = 4;
TEST(VERSIONED(Memory), addressof) {

  int x = 3;
  EXPECT_EQ(&x, preview::addressof(x));

  bizarre_object obj;
  EXPECT_EQ_TYPE(int, decltype(&obj));
  EXPECT_EQ_TYPE(bizarre_object*, decltype(preview::addressof(obj)));

#if PREVIEW_CXX_VERSION >= 17
  constexpr auto ptr = preview::addressof(global_y);
  (void)ptr;
#endif
}

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
  {
    alignas(S) unsigned char storage[sizeof(S)]{};
    S uninitialized = preview::bit_cast<S>(storage);
    preview::destroy_at(&uninitialized);
    S* ptr = preview::construct_at(preview::addressof(uninitialized), 42, 2.71f, 3.14);

    EXPECT_EQ(*ptr, (S{42, 2.71f, 3.14}));
    preview::destroy_at(ptr);
  }

  {
    alignas(std::string) unsigned char storage[sizeof(std::string)];
    auto* ptr = preview::construct_at(reinterpret_cast<std::string*>(storage));
    *ptr = "LONG STRING ENOUGH TO DISABLE SHORT STRING OPTIMIZATION";
    EXPECT_TRUE(ptr->size() > 10);

    preview::destroy_at(reinterpret_cast<std::string*>(storage));
  }
}

struct Tracer {
  int value{};
  std::function<void(int)> on_destroy;
  ~Tracer() { if (on_destroy) on_destroy(value); }
};

TEST(VERSIONED(Memory), destroy) {
  alignas(Tracer) unsigned char buffer[sizeof(Tracer) * 8];
  bool destroyed[8]{};

  for (int i = 0; i < 8; ++i)
    new(buffer + sizeof(Tracer) * i) Tracer{i, [&destroyed](int x) { destroyed[x] = true; }}; //manually construct objects

  auto ptr = reinterpret_cast<Tracer*>(buffer);

  EXPECT_TRUE(preview::ranges::equal(destroyed, preview::views::repeat(false, 8)));
  preview::destroy(ptr, ptr + 8);
  EXPECT_TRUE(preview::ranges::equal(destroyed, preview::views::repeat(true, 8)));
}

TEST(VERSIONED(Memory), destroy_at) {
  alignas(Tracer) unsigned char buffer[sizeof(Tracer) * 8];
  bool destroyed[8]{};

  for (int i = 0; i < 8; ++i)
    new(buffer + sizeof(Tracer) * i) Tracer{i, [&destroyed](int x) { destroyed[x] = true; }}; //manually construct objects

  auto ptr = reinterpret_cast<Tracer*>(buffer);

  EXPECT_TRUE(preview::ranges::equal(destroyed, preview::views::repeat(false, 8)));

  for (int i = 0; i < 8; ++i) {
    preview::destroy_at(ptr + i);
    EXPECT_TRUE(destroyed[i]);
  }
}

TEST(VERSIONED(Memory), destroy_n) {
  alignas(Tracer) unsigned char buffer[sizeof(Tracer) * 8];
  bool destroyed[8]{};

  for (int i = 0; i < 8; ++i)
    new(buffer + sizeof(Tracer) * i) Tracer{i, [&destroyed](int x) { destroyed[x] = true; }}; //manually construct objects

  auto ptr = reinterpret_cast<Tracer*>(buffer);

  EXPECT_TRUE(ranges::equal(destroyed, views::repeat(false, 8)));
  preview::destroy_n(ptr, 4);
  EXPECT_TRUE(ranges::equal(destroyed, views::concat(views::repeat(true, 4), views::repeat(false, 4))));
  preview::destroy_n(ptr + 4, 4);
  EXPECT_TRUE(ranges::equal(destroyed, views::repeat(true, 8)));
}

template<class Ptr>
struct BlockList
{
  // Predefine a memory block
  struct block;

  // Define a pointer to a memory block from the kind of pointer Ptr s
  // If Ptr is any kind of T*, block_ptr_t is block*
  // If Ptr is smart_ptr<T>, block_ptr_t is smart_ptr<block>
  using block_ptr_t = typename preview::pointer_traits<Ptr>::template rebind<block>;

  struct block
  {
    std::size_t size{};
    block_ptr_t next_block{};
  };

  block_ptr_t free_blocks;
};
TEST(VERSIONED(Memory), pointer_traits) {
  BlockList<int*> bl1;
  (void)bl1;
  // The type of bl1.free_blocks is BlockList<int*>:: block*
  EXPECT_EQ_TYPE(BlockList<int*>::block*, decltype(bl1.free_blocks));

  BlockList<std::shared_ptr<char>> bl2;
  // The type of bl2.free_blocks is
  EXPECT_EQ_TYPE(std::shared_ptr<BlockList<std::shared_ptr<char>>::block>, decltype(bl2.free_blocks));
  // std::shared_ptr<BlockList<std::shared_ptr<char>>::block>
  std::cout << bl2.free_blocks.use_count() << '\n';
}

template<class A>
auto allocator_new(A& a) {
  auto p = a.allocate(1);
  try {
    std::allocator_traits<A>::construct(a, preview::to_address(p));
  } catch (...) {
    a.deallocate(p, 1);
    throw;
  }
  return p;
}

template<class A>
void allocator_delete(A& a, typename std::allocator_traits<A>::pointer p) {
  std::allocator_traits<A>::destroy(a, preview::to_address(p));
  a.deallocate(p, 1);
}
TEST(VERSIONED(Memory), to_address) {
  {
    std::allocator<int> a;
    int* p{};
    EXPECT_NO_THROW(p = allocator_new(a));
    allocator_delete(a, p);
  }

  std::unique_ptr<int> p;
  EXPECT_EQ_TYPE(int*, decltype(preview::to_address(p)));
}

TEST(VERSIONED(Memory), pointer_cast) {
  struct A { virtual ~A() = default; };
  struct B : A {};
  struct C : A {};

  std::shared_ptr<A> p1 = std::make_shared<B>();

  // Down casting
  auto p2 = preview::dynamic_pointer_cast<B>(std::move(p1));
  EXPECT_EQ(p1.get(), nullptr);
  EXPECT_NE(p2.get(), nullptr);

  // Casting to base
  auto p3 = preview::static_pointer_cast<A>(std::move(p2));
  EXPECT_EQ(p2.get(), nullptr);
  EXPECT_NE(p3.get(), nullptr);

  // Down casting fails
  auto p4 = preview::dynamic_pointer_cast<C>(std::move(p3));
  EXPECT_NE(p3, nullptr);
  EXPECT_EQ(p4, nullptr);

  // Force casting
  auto p5 = preview::reinterpret_pointer_cast<int>(std::move(p3));
  EXPECT_EQ(p3, nullptr);
  EXPECT_NE(p5, nullptr);
}
