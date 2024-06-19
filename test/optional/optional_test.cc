//
// Created by YongGyu Lee on 2024. 6. 15..
//

#include "preview/optional.h"
#include "../gtest.h"

#include <cstring>
#include <string>

preview::optional<std::string> create(bool b) {
  if (b)
    return "Godzilla";
  return {};
}

auto create2(bool b) {
  return b ? preview::optional<std::string>{"Godzilla"} : preview::nullopt;
}

TEST(VERSIONED(optional), basics) {
  EXPECT_EQ(create(false).value_or("empty"), std::string{"empty"});
  EXPECT_EQ(create2(true), std::string{"Godzilla"});
}

TEST(VERSIONED(optional), LWG3836) {
  preview::optional<bool> op_false(false);
  preview::optional<int> op_zero(0);

  preview::optional<int> from_bool(op_false);
  preview::optional<bool> from_int(op_zero);

  EXPECT_EQ(from_bool, 0);
  EXPECT_EQ(from_int, false);
}

TEST(VERSIONED(optional), Constructor) {
  preview::optional<int> o1; // empty
  preview::optional<int> o2 = 1; // init from rvalue
  preview::optional<int> o3 = o2; // copy-constructor
  preview::optional<int> o4 = o1; // copy-constructor

  EXPECT_FALSE (o1.has_value());
  EXPECT_EQ    (o2, 1);
  EXPECT_TRUE  (o3.has_value());
  EXPECT_EQ    (o2, o3);
  EXPECT_FALSE (o4.has_value());
  EXPECT_NE    (o1, o2);
  EXPECT_NE    (o1, o3);
  EXPECT_EQ    (o1, o4);

  // calls std::string( initializer_list<CharT> ) constructor
  preview::optional<std::string> o5(preview::in_place, {'a', 'b', 'c'});
  EXPECT_EQ(o5, "abc");

  // calls std::string( size_type count, CharT ch ) constructor
  preview::optional<std::string> o6(preview::in_place, 3, 'A');
  EXPECT_EQ(o6, "AAA");

  // Move-constructed from std::string using deduction guide to pick the type
#if PREVIEW_CXX_VERSION >= 17
  preview::optional o7(std::string{"deduction"});
  EXPECT_EQ(o7, "deduction");
#endif
}

TEST(VERSIONED(optional), AssignmentOperator) {
  preview::optional<const char*> s1 = "abc", s2, s3; // constructor
  s2 = s1; // assignment
  EXPECT_EQ(std::strncmp(*s2, "abc", 10), 0);

  s1 = "def"; // decaying assignment (U = char[4], T = const char*)
  EXPECT_EQ(std::strncmp(*s1, "def", 10), 0);

  s1 = s3;
  EXPECT_FALSE(s1.has_value());
}

TEST(VERSIONED(optional), OperatorArrowAndIndirect) {
  using namespace std::string_literals;

  preview::optional<int> opt1 = 1;
  EXPECT_NO_THROW(*opt1);
  EXPECT_EQ(*opt1, 1);

  *opt1 = 2;
  EXPECT_EQ(*opt1, 2);

  preview::optional<std::string> opt2 = "abc"s;
  EXPECT_EQ(opt2->size(), 3);

  // You can "take" the contained value by calling operator* on an rvalue to optional

  auto taken = *std::move(opt2);
  std::cout << "taken: " << std::quoted(taken) << "\n"
                                                  "opt2: " << std::quoted(*opt2) << ", size: " << opt2->size()  << '\n';
}

struct NoMove {
  NoMove() = default;
  NoMove(const NoMove&) {
    copied += 1;
  }
  NoMove(NoMove&&) = delete;
  NoMove& operator=(const NoMove&) = default;
  static int copied;
};
int NoMove::copied{};

TEST(VERSIONED(optional), copy_over_move) {
  preview::optional<NoMove> op1{preview::in_place};
  preview::optional<NoMove> op2;
  std::swap(op1, op2); // copy is selected for non-movable object
  EXPECT_EQ(NoMove::copied, 2);
}
