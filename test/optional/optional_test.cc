//
// Created by YongGyu Lee on 2024. 6. 15..
//

#include "preview/optional.h"
#include "../gtest.h"

#if PREVIEW_CXX_VERSION >= 17
#include <charconv>
#endif
#include <cstring>
#include <string>
#include <unordered_set>
#include <vector>

#include "preview/__algorithm/ranges/equal.h"
#include "preview/__ranges/views/repeat.h"
#include "preview/__ranges/views/transform.h"

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

TEST(VERSIONED(optional), operator_bool) {
  preview::optional<int> opt;
  EXPECT_FALSE(opt.has_value());

  opt = 43;
  EXPECT_TRUE(opt);
  EXPECT_EQ(opt, 43);

  opt.reset();
  EXPECT_FALSE(opt.has_value());
}

TEST(VERSIONED(optional), value) {
  preview::optional<int> opt = {};
  EXPECT_THROW(opt.value(), preview::bad_optional_access);
  EXPECT_THROW(opt.value() = 42, preview::bad_optional_access);

  opt = 43;
  EXPECT_NO_THROW(*opt);
  EXPECT_EQ(*opt, 43);

  opt.value() = 44;
  EXPECT_NO_THROW(opt.value());
  EXPECT_EQ(opt.value(), 44);
}

TEST(VERSIONED(optional), value_or) {
  EXPECT_EQ(preview::optional<int>{3}.value_or(4), 3);
  EXPECT_EQ(preview::optional<int>{}.value_or(4), 4);
}

TEST(VERSIONED(optional), and_then) {
#if PREVIEW_CXX_VERSION >= 17
  using namespace std::string_literals;

  const std::vector<preview::optional<std::string>> v = {
      "1234", "15 foo", "bar", "42", "5000000000", " 5", preview::nullopt, "-43"
  };

  auto to_int = [](std::string_view sv) -> preview::optional<int> {
    int r{};
    auto [ptr, ec]{std::from_chars(sv.data(), sv.data() + sv.size(), r)};
    if (ec == std::errc())
      return r;
    else
      return preview::nullopt;
  };

  const std::vector<std::string> answer = {
    "1235", "16", "NaN", "43", "NaN", "NaN", "NaN", "-42"
  };
  auto it = answer.begin();

  for (auto&& x : v | preview::views::transform(
    [=](auto&& o) {
      // debug print the content of input optional<string>
      std::cout << std::left << std::setw(13)
                << std::quoted(o.value_or("nullopt")) << " -> ";

      return o
        // if optional is nullopt convert it to optional with "" string
        .or_else([]{ return preview::optional{""s}; })
          // flatmap from strings to ints (making empty optionals where it fails)
        .and_then(to_int)
          // map int to int + 1
        .transform([](int n) { return n + 1; })
          // convert back to strings
        .transform([](int n) { return std::to_string(n); })
          // replace all empty optionals that were left by
          // and_then and ignored by transforms with "NaN"
        .value_or("NaN"s);
    })) {
    EXPECT_EQ(x, *it);
    ++it;
  }
#endif
}

struct A {};
struct B {};
struct C {};
struct D {};

auto A_to_B(A) -> B { return {}; }
auto B_to_C(B) -> C { return {}; }
auto C_to_D(C) -> D { return {}; }

preview::optional<D> try_transform_A_to_D(preview::optional<A> o_A)
{
  return o_A.transform(A_to_B)
            .transform(B_to_C)
            .transform(C_to_D);
};

TEST(VERSIONED(optional), transform) {
  EXPECT_TRUE(try_transform_A_to_D( A{} ));
  EXPECT_FALSE(try_transform_A_to_D( {} ));
}

TEST(VERSIONED(optional), or_else) {
  using maybe_int = preview::optional<int>;

  auto valueless = [] {
    return maybe_int{0};
  };

  maybe_int x;
  EXPECT_EQ(x.or_else(valueless).value(), 0);

  x = 42;
  EXPECT_EQ(x.or_else(valueless).value(), 42);

  x.reset();
  EXPECT_EQ(x.or_else(valueless).value(), 0);
}

TEST(VERSIONED(optional), swap) {
  preview::optional<std::string> opt1("First example text");
  preview::optional<std::string> opt2("2nd text");

  opt1.swap(opt2);
  EXPECT_EQ(opt1, "2nd text");
  EXPECT_EQ(opt2, "First example text");

  // Swap with only 1 set
  opt1 = "Lorem ipsum dolor sit amet, consectetur tincidunt.";
  opt2.reset();

  opt1.swap(opt2);
  EXPECT_EQ(opt1.value_or(""), "");
  EXPECT_EQ(opt2.value_or(""), "Lorem ipsum dolor sit amet, consectetur tincidunt.");
}

struct A2 {
  static int constructed, destructed, copy_constructed, move_constructed, copy_assigned, move_assigned;

  std::string s;
  A2(std::string str) : s(std::move(str))  { ++constructed; }
  ~A2() { ++destructed; }
  A2(const A2& o) : s(o.s) { ++copy_constructed; }
  A2(A2&& o) : s(std::move(o.s)) { ++move_constructed; }

  A2& operator=(const A2& other)
  {
    s = other.s;
    ++copy_assigned;
    return *this;
  }

  A2& operator=(A2&& other)
  {
    s = std::move(other.s);
    ++move_assigned;
    return *this;
  }

  static void ResetCounter() {
    constructed = destructed = copy_constructed = move_constructed = copy_assigned = move_assigned = 0;
  }
};
int A2::constructed{};
int A2::destructed{};
int A2::copy_constructed{};
int A2::move_constructed{};
int A2::copy_assigned{};
int A2::move_assigned{};

TEST(VERSIONED(optional), reset) {
  preview::optional<A2> opt;
  EXPECT_EQ(A2::constructed, 0);

  opt = A2("Lorem ipsum dolor sit amet, consectetur adipiscing elit nec.");
  EXPECT_EQ(A2::constructed, 1);
  EXPECT_EQ(A2::copy_constructed, 0);
  EXPECT_EQ(A2::move_constructed, 1);
  EXPECT_EQ(A2::copy_assigned, 0);
  EXPECT_EQ(A2::move_assigned, 0);
  EXPECT_EQ(A2::destructed, 1);

  opt.reset();
  EXPECT_EQ(A2::destructed, 2);
  A2::ResetCounter();
}

TEST(VERSIONED(optional), emplace) {
  {
    preview::optional<A2> opt;
    opt = A2("Lorem ipsum dolor sit amet, consectetur adipiscing elit nec.");

    A2::ResetCounter();
    opt.emplace("Lorem ipsum dolor sit amet, consectetur efficitur.");
    EXPECT_EQ(A2::constructed, 1);
    EXPECT_EQ(A2::copy_constructed, 0);
    EXPECT_EQ(A2::move_constructed, 0);
    EXPECT_EQ(A2::copy_assigned, 0);
    EXPECT_EQ(A2::move_assigned, 0);
    EXPECT_EQ(A2::destructed, 1);
  }
  EXPECT_EQ(A2::destructed, 2);
  A2::ResetCounter();
}

TEST(VERSIONED(optional), make_optional) {
  auto op1 = preview::make_optional<std::vector<char>>({'a','b','c'});
  EXPECT_TRUE(preview::ranges::equal(op1.value(), {'a', 'b', 'c'}));

  auto op2 = preview::make_optional<std::vector<int>>(5, 2);
  EXPECT_TRUE(preview::ranges::equal(*op2, preview::views::repeat(2, 5)));

  std::string str{"hello world"};
  auto op3 = preview::make_optional<std::string>(std::move(str));
  EXPECT_EQ(op3.value_or("empty value"), "hello world");
  EXPECT_EQ(str, "");
}

TEST(VERSIONED(optional), swap_global) {
  preview::optional<std::string> a{"AAAA"}, b{"BBBB"};

  std::swap(a, b);
  EXPECT_EQ(a, "BBBB");
  EXPECT_EQ(b, "AAAA");

  a.reset();
  EXPECT_EQ(a.value_or("NULL"), "NULL");
  EXPECT_EQ(b.value_or("NULL"), "AAAA");

  std::swap(a, b);
  EXPECT_EQ(a.value_or("NULL"), "AAAA");
  EXPECT_EQ(b.value_or("NULL"), "NULL");
}

TEST(VERSIONED(optional), hash) {
  using namespace std::literals;
  using OptStr = preview::optional<std::string>;

  // hash<optional> makes it possible to use unordered_set
  std::unordered_set<OptStr> s = {
      "ABC"s, "abc"s, preview::nullopt, "def"s
  };

  for (const auto& o : s) {
    if (o.has_value())
      EXPECT_EQ(std::hash<OptStr>{}(o), std::hash<std::string>{}(*o));
    else
      EXPECT_EQ(std::hash<OptStr>{}(o), 0 /* We use zero value */);
  }
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
