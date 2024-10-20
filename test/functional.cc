#include "preview/core.h"
#include "preview/functional.h"

#include <functional>
#include <type_traits>

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

template<typename T>
void foo(std::reference_wrapper<T> x) {}

struct A {
  explicit A(int data) : data(data) {}

  int data{};
  int mem_fn() { return data; }

  friend constexpr bool operator==(const A& x, const A& y) {
    return x.data == y.data;
  }
  friend constexpr bool operator<(const A& x, const A& y) {
    return x.data < y.data;
  }
};

TEST(VERSIONED(Functional), reference_wrapper) {
  A x{10};
  auto r = preview::ref(x);
  EXPECT_EQ(r, /* == */ A{10}); EXPECT_EQ(A{10}, /* == */ r);
  EXPECT_NE(r, /* != */ A{0 }); EXPECT_NE(A{0 }, /* != */ r);
  EXPECT_GT(r, /* >  */ A{9 }); EXPECT_GT(A{11}, /* >  */ r);
  EXPECT_GE(r, /* >= */ A{9 }); EXPECT_GE(A{11}, /* >= */ r);
  EXPECT_LT(r, /* <  */ A{11}); EXPECT_LT(A{9 }, /* <  */ r);
  EXPECT_LE(r, /* <= */ A{11}); EXPECT_LE(A{9 }, /* <= */ r);

  EXPECT_EQ(r, r);
  EXPECT_EQ(r, preview::cref(x)); EXPECT_EQ(preview::cref(x), r);

  A y{20};
  EXPECT_NE(r, preview::ref(y)); EXPECT_NE(r, preview::cref(y));
  EXPECT_GT(preview::ref(y), r); EXPECT_GT(preview::cref(y), r);
  EXPECT_GE(preview::ref(y), r); EXPECT_GE(preview::cref(y), r);
  EXPECT_LT(r, preview::ref(y)); EXPECT_LT(r, preview::cref(y));
  EXPECT_LE(r, preview::ref(y)); EXPECT_LE(r, preview::cref(y));

  EXPECT_EQ(preview::invoke(&A::data, r), 10);
  preview::invoke(&A::data, r) += 5;
  EXPECT_EQ(preview::invoke(&A::data, r), 15);
  EXPECT_EQ(preview::invoke(&A::mem_fn, r), 15);

#if PREVIEW_CXX_VERSION >= 17
  std::invoke(&A::data, r) -= 5;
  EXPECT_EQ(std::invoke(&A::data, r), 10);
  EXPECT_EQ(std::invoke(&A::mem_fn, r), 10);
#endif

  {
    auto cr = preview::cref(x);
    A z{30};

    EXPECT_EQ(r,  std::ref(x));      EXPECT_EQ(std::ref(x) , r );
    EXPECT_EQ(r,  std::cref(x));     EXPECT_EQ(std::cref(x), r );

    (void)(cr != std::ref(x));
    (void)(std::ref(x) == cr);

    EXPECT_EQ(cr, std::ref(x));      EXPECT_EQ(std::ref(x) , cr);
    EXPECT_EQ(cr, std::cref(x));     EXPECT_EQ(std::cref(x), cr);

    EXPECT_NE(r,  std::ref(y));      EXPECT_NE(std::ref(y) , r );
    EXPECT_NE(r,  std::cref(y));     EXPECT_NE(std::cref(y), r );
    EXPECT_NE(cr, std::ref(y));      EXPECT_NE(std::ref(y) , cr);
    EXPECT_NE(cr, std::cref(y));     EXPECT_NE(std::cref(y), cr);

    // x < y < z

    EXPECT_GT(preview::ref(z),  std::ref(y));   EXPECT_GT(std::ref(y),  preview::ref(x));
    EXPECT_GT(preview::ref(z),  std::cref(y));  EXPECT_GT(std::cref(y), preview::ref(x));
    EXPECT_GT(preview::cref(z), std::ref(y));   EXPECT_GT(std::ref(y),  preview::cref(x));
    EXPECT_GT(preview::cref(z), std::cref(y));  EXPECT_GT(std::cref(y), preview::cref(x));

    EXPECT_GE(preview::ref(z),  std::ref(y));   EXPECT_GE(std::ref(y),  preview::ref(x));
    EXPECT_GE(preview::ref(z),  std::cref(y));  EXPECT_GE(std::cref(y), preview::ref(x));
    EXPECT_GE(preview::cref(z), std::ref(y));   EXPECT_GE(std::ref(y),  preview::cref(x));
    EXPECT_GE(preview::cref(z), std::cref(y));  EXPECT_GE(std::cref(y), preview::cref(x));

    EXPECT_LT(preview::ref(x),  std::ref(y));   EXPECT_LT(std::ref(y),  preview::ref(z));
    EXPECT_LT(preview::ref(x),  std::cref(y));  EXPECT_LT(std::cref(y), preview::ref(z));
    EXPECT_LT(preview::cref(x), std::ref(y));   EXPECT_LT(std::ref(y),  preview::cref(z));
    EXPECT_LT(preview::cref(x), std::cref(y));  EXPECT_LT(std::cref(y), preview::cref(z));

    EXPECT_LE(preview::ref(x),  std::ref(y));   EXPECT_LE(std::ref(y),  preview::ref(z));
    EXPECT_LE(preview::ref(x),  std::cref(y));  EXPECT_LE(std::cref(y), preview::ref(z));
    EXPECT_LE(preview::cref(x), std::ref(y));   EXPECT_LE(std::ref(y),  preview::cref(z));
    EXPECT_LE(preview::cref(x), std::cref(y));  EXPECT_LE(std::cref(y), preview::cref(z));
  }

#if PREVIEW_CONFORM_CXX20_STANDARD
  (void)foo(r);
#endif

//  preview::simple_common_reference_t<int&, preview::reference_wrapper<int>&>

  using T = int&;
  using R = preview::reference_wrapper<int>&;

  using bcr = preview::basic_common_reference<
      int,
      preview::reference_wrapper<int>,
      preview::detail::basic_common_reference_qual_gen<T>::qual,
      preview::detail::basic_common_reference_qual_gen<R>::qual>;
  using t = typename bcr::type;
//
//  static_assert(preview::detail::common_reference_tag_2<T, R>::value);
//
//
  static_assert(std::is_same<preview::common_reference_t<int&, int&>, int&>::value, "");
  static_assert(std::is_same<preview::common_reference_t<int&, preview::reference_wrapper<int>&>, int&>::value, "");
  static_assert(std::is_same<preview::common_reference_t<int&, preview::reference_wrapper<int>&>, int&>::value, "");
  static_assert(std::is_same<preview::common_reference_t<int&,     std::reference_wrapper<int>&>, int&>::value, "");
//  static_assert(std::is_same_v<    std::common_reference_t<int&, preview::reference_wrapper<int>&>, int&>);

}
