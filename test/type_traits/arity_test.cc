//
// Created by YongGyu Lee on 2024. 6. 13..
//

#include "preview/__type_traits/arity.h"

#include <functional>
#include <type_traits>

#include "../test_utils.h"

template<typename... Args>
struct functor {
  void operator()(Args...) {}
};

TEST(VERSIONED(arity), functor) {
  EXPECT_EQ(preview::arity_v<functor<>>, 0);
  EXPECT_EQ(preview::arity_v<functor<int>>, 1);
  EXPECT_EQ((preview::arity_v<functor<int, float>>), 2);
}

TEST(VERSIONED(arity), lambda) {
  auto l0 = [](){};(void)l0;
  auto l1 = [](auto){};(void)l1;
  auto l2 = [](auto, auto){};(void)l2;
  EXPECT_EQ(preview::arity_v<decltype(l0)>, 0);
  EXPECT_EQ(preview::arity_v<decltype(l1)>, 1);
  EXPECT_EQ(preview::arity_v<decltype(l2)>, 2);
}

void f0() {}
void f1(int) {}
void f2(int, void(*)(int)) {}

TEST(VERSIONED(arity), function) {
  EXPECT_EQ(preview::arity_v<decltype(f0)>, 0);
  EXPECT_EQ(preview::arity_v<decltype(f1)>, 1);
  EXPECT_EQ(preview::arity_v<decltype(f2)>, 2);
  EXPECT_EQ(preview::arity_v<decltype(&f0)>, 0);
  EXPECT_EQ(preview::arity_v<decltype(&f1)>, 1);
  EXPECT_EQ(preview::arity_v<decltype(&f2)>, 2);
}

struct foo {
  void f0(){}
  void f1(int)const{}
};

TEST(VERSIONED(arity), member_function) {
  EXPECT_EQ(preview::arity_v<decltype(&foo::f0)>, 0);
  EXPECT_EQ(preview::arity_v<decltype(&foo::f1)>, 1);
}

TEST(VERSIONED(arity), std_function) {
  EXPECT_EQ(preview::arity_v<std::function<void()>>, 0);
  EXPECT_EQ(preview::arity_v<std::function<void(int)>>, 1);
  EXPECT_EQ(preview::arity_v<std::function<void(int, int)>>, 2);
}
