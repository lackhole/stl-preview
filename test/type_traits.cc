#include "preview/type_traits.h"
#include "gtest.h"

TEST(VERSIONED(type_traits), common_type) {
  EXPECT_TRUE ((std::is_same<preview::common_type_t<int, int>, int>::value));
  EXPECT_TRUE ((std::is_same<preview::common_type_t<int, double>, double>::value));
  EXPECT_TRUE ((std::is_same<preview::common_type_t<double, int>, double>::value));
  EXPECT_TRUE ((std::is_same<preview::common_type_t<double, double>, double>::value));

  EXPECT_TRUE ((std::is_same<preview::common_type_t<std::pair<int, int>, std::pair<int, double>>, std::pair<int, double>>::value));
  EXPECT_TRUE ((std::is_same<preview::common_type_t<std::pair<int, int>, std::tuple<int, double>>, std::tuple<int, double>>::value));
  EXPECT_TRUE ((std::is_same<preview::common_type_t<std::tuple<int, int>, std::pair<int, double>>, std::tuple<int, double>>::value));
  EXPECT_TRUE ((std::is_same<preview::common_type_t<std::tuple<int, int>, std::tuple<int, double>>, std::tuple<int, double>>::value));
}

TEST(VERSIONED(type_traits), type_identity) {
  EXPECT_TRUE  ((std::is_same<preview::type_identity<int>::type, preview::type_identity_t<int>>::value));
  EXPECT_TRUE  ((std::is_same<preview::type_identity_t<int>, int>::value));
  EXPECT_TRUE  ((std::is_same<preview::type_identity_t<void()>, void()>::value));
  EXPECT_TRUE  ((std::is_same<preview::type_identity_t<decltype(std::cout)>, decltype(std::cout)>::value));
  EXPECT_TRUE  ((std::is_same<preview::type_identity_t<void()&&>, void()&&>::value));
}

TEST(VERSIONED(type_traits), void_t) {
  EXPECT_TRUE  ((std::is_void<preview::void_t<>>::value));
  EXPECT_TRUE  ((std::is_void<preview::void_t<int>>::value));
  EXPECT_TRUE  ((std::is_void<preview::void_t<int, float, std::string>>::value));
}

TEST(VERSIONED(type_traits), is_swappable) {
  struct e { e(const e &) = delete; };
  EXPECT_TRUE  (preview::is_swappable_v<std::unique_ptr<int>>);
  EXPECT_TRUE  ((preview::is_swappable_with_v<int&, int&>));
  EXPECT_TRUE  (preview::is_swappable_v<int>);
  EXPECT_FALSE (preview::is_swappable_v<int[]>);
  EXPECT_TRUE  (preview::is_swappable_v<int[3]>);
  EXPECT_FALSE (preview::is_swappable_v<e>);
  EXPECT_FALSE (preview::is_swappable_v<void() &&>);
}

TEST(VERSIONED(type_traits), is_nothrow_swappable) {
  struct e2 { e2(const e2 &) noexcept(false); };
  EXPECT_TRUE  (preview::is_nothrow_swappable_v<int>);
  EXPECT_FALSE (preview::is_nothrow_swappable_v<e2>);
}

TEST(VERSIONED(type_traits), invoke_result) {
  using x = preview::invoke_result<int>;
  using y = preview::invoke_result<void()&&, std::nullptr_t, void()>;

  struct foo {};
  struct incomplete;

  EXPECT_TRUE((std::is_void<preview::invoke_result_t<void()>>::value));
  EXPECT_TRUE((std::is_void<preview::invoke_result_t<void(*)()>>::value));
  EXPECT_TRUE((std::is_void<preview::invoke_result_t<void(foo::*)(), foo&>>::value));
  EXPECT_TRUE((std::is_void<preview::invoke_result_t<void(foo::*)(), foo*>>::value));
# if PREVIEW_CXX_VERSION >= 17
  EXPECT_TRUE((std::is_same_v< preview::invoke_result_t<void(foo::*)(), foo>,
                                   std::invoke_result_t<void(foo::*)(), foo>>));
# endif


  EXPECT_TRUE((std::is_same<preview::invoke_result_t<int()>, int>::value));
  EXPECT_TRUE((std::is_same<preview::invoke_result_t<foo()>, foo>::value));
  EXPECT_TRUE((std::is_same<preview::invoke_result_t<void(incomplete[]), incomplete[3]>, void>::value));

# if PREVIEW_CXX_VERSION >= 14
  auto lambda = [](auto x) -> std::conditional_t<std::is_integral<decltype(x)>::value, int, float> { return 0; };
  EXPECT_TRUE((std::is_same<preview::invoke_result_t<decltype(lambda), int>, int>::value));
  EXPECT_TRUE((std::is_same<preview::invoke_result_t<decltype(lambda), float>, float>::value));
# endif

  struct c {
    int operator()();
    void operator()(int);
  };
  EXPECT_TRUE((std::is_same<preview::invoke_result_t<c>, int>::value));
  EXPECT_TRUE((std::is_same<preview::invoke_result_t<c, float>, void>::value));
  EXPECT_TRUE((std::is_same<void, preview::invoke_result_t<c, int>>::value));
}

TEST(VERSIONED(type_traits), is_invocable) {
  EXPECT_TRUE((preview::is_invocable<void()>::value));
  EXPECT_TRUE((preview::is_invocable<void(*)()>::value));
  EXPECT_TRUE((preview::is_invocable<void(&)()>::value));
  EXPECT_TRUE((preview::is_invocable<void(int), float>::value));
  EXPECT_TRUE((preview::is_invocable<void(bool), int>::value));
  EXPECT_TRUE((preview::is_invocable<void(int), bool>::value));
  EXPECT_TRUE((preview::is_invocable<void(int), preview::bool_constant<true>>::value));

  struct a {};
  struct b : a {};

  EXPECT_TRUE((preview::is_invocable<void(a&), b&>::value));
}

TEST(VERSIONED(type_traits), disjunction) {
  EXPECT_FALSE ((preview::disjunction_v<>));
  EXPECT_FALSE ((preview::disjunction_v<std::false_type>));
  EXPECT_FALSE ((preview::disjunction_v<std::false_type, std::false_type>));
  EXPECT_TRUE  ((preview::disjunction_v<std::false_type, std::false_type, std::false_type, std::true_type>));
  EXPECT_TRUE  ((preview::disjunction_v<std::true_type>));

  using preview::int_constant;
  EXPECT_TRUE  ((preview::disjunction<int_constant<0>, int_constant<0>, int_constant<2>, int_constant<1>>::value == 2));
}

TEST(VERSIONED(type_traits), negation) {
  EXPECT_TRUE((preview::negation<std::false_type>::value == true));
  EXPECT_TRUE((preview::negation<std::true_type>::value == false));
  EXPECT_TRUE((preview::negation<std::false_type>::value == preview::negation_v<std::false_type>));
  EXPECT_TRUE((preview::negation<std::true_type>::value == preview::negation_v<std::true_type>));
}

template<typename T>
void reset (T&& z) {
  decltype(auto) r = preview::unwrap_reference_t<T>(z);
  r = 0;
};

TEST(VERSIONED(type_traits), unwrap_reference) {
  EXPECT_TRUE_TYPE(std::is_same<preview::unwrap_reference_t<int>, int>);
  EXPECT_TRUE_TYPE(std::is_same<preview::unwrap_reference_t<const int>, const int>);
  EXPECT_TRUE_TYPE(std::is_same<preview::unwrap_reference_t<int&>, int&>);
  EXPECT_TRUE_TYPE(std::is_same<preview::unwrap_reference_t<int&&>, int&&>);
  EXPECT_TRUE_TYPE(std::is_same<preview::unwrap_reference_t<int*>, int*>);

  {
    using T = std::reference_wrapper<int>;
    using X = preview::unwrap_reference_t<T>;
    EXPECT_TRUE_TYPE(std::is_same<X, int&>);
  }
  {
    using T = std::reference_wrapper<int&>;
    using X = preview::unwrap_reference_t<T>;
    EXPECT_TRUE_TYPE(std::is_same<X, int&>);
  }

  EXPECT_TRUE_TYPE(std::is_same<preview::unwrap_ref_decay_t<int>, int>);
  EXPECT_TRUE_TYPE(std::is_same<preview::unwrap_ref_decay_t<const int>, int>);
  EXPECT_TRUE_TYPE(std::is_same<preview::unwrap_ref_decay_t<const int&>, int>);

  {
    using T = std::reference_wrapper<int&&>;
    using X = preview::unwrap_ref_decay_t<T>;
    EXPECT_TRUE_TYPE(std::is_same<X, int&>);
  }

  {
    int x = 1;
    reset(x);
    EXPECT_EQ(x, 0);

    int y = 2;
    reset(std::ref(y));
    EXPECT_EQ(y, 0);
  }
}
