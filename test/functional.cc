#include "preview/core.h"
#include "preview/functional.h"
#include "preview/string_view.h"

#include "gtest.h"

int minus(int a, int b) {
  return a - b;
}

struct S {
  int val;
  int minus(int arg) const noexcept { return val - arg; }
};
TEST(VERSIONED(Functional), bind_partial) {
  auto fifty_minus = preview::bind_front(minus, 50);
  EXPECT_EQ(fifty_minus(3), 47); // equivalent to: minus(50, 3) == 47

  auto member_minus = preview::bind_front(&S::minus, S{50});
  auto x = member_minus(3);
  EXPECT_EQ(member_minus(3), 47); //: S tmp{50}; tmp.minus(3) == 47

  // Noexcept-specification is preserved:
  static_assert(!noexcept(fifty_minus(3)), "");
#if PREVIEW_CXX_VERSION >= 17
  static_assert(noexcept(member_minus(3)), "");
#endif

  // Binding of a lambda:
  PREVIEW_CONSTEXPR_AFTER_CXX20 auto plus = [](int a, int b) { return a + b; };
  auto forty_plus = preview::bind_front(plus, 40);
  EXPECT_EQ(forty_plus(7), 47); // equivalent to: plus(40, 7) == 47

  // bind_back
  PREVIEW_CONSTEXPR_AFTER_CXX20 auto madd = [](int a, int b, int c) { return a * b + c; };
  auto mul_plus_seven = preview::bind_back(madd, 7);
  EXPECT_EQ(mul_plus_seven(4, 10), 47); //: madd(4, 10, 7) == 47

#if PREVIEW_CXX_VERSION >= 17
  auto fifty_minus_cpp26 = preview::bind_front<minus>(50);
  EXPECT_EQ(fifty_minus_cpp26(3), 47);

  auto member_minus_cpp26 = preview::bind_front<&S::minus>(S{50});
  EXPECT_EQ(member_minus_cpp26(3), 47);
#else
#if defined(__GNUC__) && !defined(__clang__)
  auto fifty_minus_cpp26 = preview::bind_front<decltype(&minus), minus>(50);
#else
  auto fifty_minus_cpp26 = preview::bind_front<decltype(minus), minus>(50);
#endif
  EXPECT_EQ(fifty_minus_cpp26(3), 47);

  auto member_minus_cpp26 = preview::bind_front<decltype(&S::minus), &S::minus>(S{50});
  EXPECT_EQ(member_minus_cpp26(3), 47);
#endif

#if PREVIEW_CXX_VERSION >= 20 && \
    (!defined(__linux) || (defined(__clang_major__) && __clang_major__ > 11)) &&       \
    (!PREVIEW_ANDROID || (defined(PREVIEW_NDK_VERSION_MAJOR) && PREVIEW_NDK_VERSION_MAJOR >= 26))
  auto forty_plus_cpp26 = preview::bind_front<plus>(40);
  EXPECT_EQ(forty_plus_cpp26(7), 47);

  auto mul_plus_seven_cpp26 = preview::bind_back<madd>(7);
  EXPECT_EQ(mul_plus_seven_cpp26(4, 10), 47);
#else
  // lambda expression is non-literal before C++17
#endif
}

TEST(VERSIONED(Functional), default_searcher) {
  PREVIEW_CONSTEXPR_AFTER_CXX17 preview::string_view in =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed "
      "do eiusmod tempor incididunt ut labore et dolore magna aliqua";

  const preview::string_view needle{"pisci"};

#if PREVIEW_CXX_VERSION >= 17
  auto it = std::search(in.begin(), in.end(), preview::default_searcher(needle.begin(), needle.end()));
  EXPECT_NE(it, in.end());
  EXPECT_EQ(it - in.begin(), 43);
#endif
}

bool is_same(int a, int b) noexcept {
  return a == b;
}

struct S2 {
  int val;
  bool is_same(int arg) const noexcept { return val == arg; }
};

TEST(VERSIONED(Functional), not_fn) {
  auto is_differ = preview::not_fn(is_same);
  EXPECT_FALSE(is_differ(8, 8)); // equivalent to: !is_same(8, 8) == false
  EXPECT_TRUE (is_differ(6, 9)); // equivalent to: !is_same(8, 0) == true

  // Using with a member function:
  auto member_differ = preview::not_fn(&S2::is_same);
  EXPECT_FALSE(member_differ(S2{3}, 3)); //: S tmp{6}; !tmp.is_same(6) == false

  // Noexcept-specification is preserved:
  EXPECT_EQ(noexcept(is_differ), noexcept(is_same));
  EXPECT_EQ(noexcept(member_differ), noexcept(&S2::is_same));

  // Using with a function object:
  PREVIEW_CONSTEXPR_AFTER_CXX20 auto same = [](int a, int b) { return a == b; };
  auto differ = preview::not_fn(same);
  EXPECT_TRUE (differ(1, 2)); //: !same(1, 2) == true
  EXPECT_FALSE(differ(2, 2)); //: !same(2, 2) == false

#if PREVIEW_CXX_VERSION >= 17
  auto is_differ_cpp26 = preview::not_fn<is_same>();
  EXPECT_FALSE(is_differ_cpp26(8, 8));
  EXPECT_TRUE (is_differ_cpp26(6, 9));

  auto member_differ_cpp26 = preview::not_fn<&S2::is_same>();
  EXPECT_FALSE(member_differ_cpp26(S2{3}, 3));
#else
  #if defined(__GNUC__) && !defined(__clang__)
  auto is_differ_cpp26 = preview::not_fn<decltype(&is_same), is_same>();
#else
  auto is_differ_cpp26 = preview::not_fn<decltype(is_same), is_same>();
#endif
  EXPECT_FALSE(is_differ_cpp26(8, 8));
  EXPECT_TRUE (is_differ_cpp26(6, 9));

  auto member_differ_cpp26 = preview::not_fn<decltype(&S2::is_same), &S2::is_same>();
  EXPECT_FALSE(member_differ_cpp26(S2{3}, 3));
#endif

#if PREVIEW_CXX_VERSION >= 20 && \
    (!defined(__linux) || (defined(__clang_major__) && __clang_major__ > 11)) &&       \
    (!PREVIEW_ANDROID || (defined(PREVIEW_NDK_VERSION_MAJOR) && PREVIEW_NDK_VERSION_MAJOR >= 26))
  auto differ_cpp26 = preview::not_fn<same>();
  static_assert(differ_cpp26(1, 2) == true);
  static_assert(differ_cpp26(2, 2) == false);
#endif
}
