#include "preview/core.h"
#include "preview/functional.h"
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
