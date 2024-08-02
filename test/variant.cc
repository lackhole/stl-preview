#include "preview/variant.h"

#include "gtest.h"

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "preview/algorithm.h"
#include "preview/functional.h"
#include "preview/ranges.h"
#include "preview/type_traits.h"

namespace ranges = preview::ranges;
namespace views = preview::views;

template<typename T, typename V, typename = void>
struct is_gettable : std::false_type {};
template<typename T, typename V>
struct is_gettable<T, V, preview::void_t<decltype(std::get<T>(std::declval<V>()))>> : std::true_type {};

template<std::size_t I, typename V, typename = void>
struct is_gettable_i : std::false_type {};
template<std::size_t I, typename V>
struct is_gettable_i<I, V, preview::void_t<decltype(std::get<I>(std::declval<V>()))>> : std::true_type {};

template<typename T, typename V>
constexpr bool gettable(V&& v) { return is_gettable<T, V&&>::value; }
template<std::size_t I, typename V>
constexpr bool gettable(V&& v) { return is_gettable_i<I, V&&>::value; }

TEST(VERSIONED(Variant), basics) {
  preview::variant<int, float> v, w;
  v = 42; // v contains int

  EXPECT_EQ(std::get<int>(v), 42);
  w = std::get<int>(v);
  EXPECT_EQ(std::get<int>(w), 42);
  w = std::get<0>(v); // same effect as the previous line
  EXPECT_EQ(std::get<0>(v), 42);
  w = v; // same effect as the previous line
  EXPECT_EQ(w, v);

  EXPECT_FALSE(gettable<double>(v)); // error: no double in [int, float]
  EXPECT_FALSE(gettable<3>(v));      // error: valid index values are 0 and 1

  EXPECT_THROW(std::get<float>(w), preview::bad_variant_access);

  using namespace std::literals;

  preview::variant<std::string> x("abc");
  // converting constructors work when unambiguous
  x = "def"; // converting assignment also works when unambiguous

  preview::variant<std::string, void const*> y("abc");
  // casts to void const* when passed a char const*
  EXPECT_TRUE(preview::holds_alternative<void const*>(y));

  y = "xyz"s;
  EXPECT_TRUE(preview::holds_alternative<std::string>(y));
}

TEST(VERSIONED(Variant), ctor) {
  using vector_t = std::vector<int>;

  // value-initializes first alternative
  preview::variant<int, std::string> var0;
  EXPECT_TRUE(preview::holds_alternative<int>(var0));
  EXPECT_EQ(var0.index(), 0);
  EXPECT_EQ(std::get<int>(var0), 0);

  // initializes first alternative with std::string{"STR"};
  preview::variant<std::string, int> var1{"STR"};
  EXPECT_EQ(var1.index(), 0);
  EXPECT_EQ(std::get<std::string>(var1), "STR");

  // initializes second alternative with int == 42;
  preview::variant<std::string, int> var2{42};
  EXPECT_TRUE(preview::holds_alternative<int>(var2));
  EXPECT_EQ(std::get<int>(var2), 42);

  // initializes third alternative with std::string{4, 'A'};
  preview::variant<vector_t, float, std::string> var3{preview::in_place_type<std::string>, 4, 'A'};
  EXPECT_EQ(var3.index(), 2);
  EXPECT_EQ(std::get<std::string>(var3), "AAAA");

  // initializes second alternative with std::vector{1,2,3,4,5};
  preview::variant<std::string, vector_t, char> var4{preview::in_place_type<vector_t>, {1, 2, 3, 4, 5}};
  EXPECT_EQ(var4.index(), 1);
  EXPECT_TRUE((preview::ranges::equal(std::get<vector_t>(var4), {1, 2, 3, 4, 5})));

  // initializes first alternative with std::string{"ABCDE", 3};
  preview::variant<std::string, vector_t, bool> var5 {preview::in_place_index<0>, "ABCDE", 3};
  EXPECT_EQ(var5.index(), 0);
  EXPECT_EQ(std::get<std::string>(var5), "ABC");

  // initializes second alternative with std::vector(4, 42);
  preview::variant<std::string, vector_t, char> var6 {preview::in_place_index<1>, 4, 42};
  EXPECT_TRUE(preview::holds_alternative<vector_t>(var6));
  EXPECT_TRUE((preview::ranges::equal(std::get<vector_t>(var6), {42, 42, 42, 42})));
}

int x_dtor = 0;
int y_dtor = 0;
struct X { ~X() { ++x_dtor; } };
struct Y { ~Y() { ++y_dtor; } };

TEST(VERSIONED(Variant), dtor) {

  {
    preview::variant<X,Y> var;
  }
  EXPECT_TRUE(x_dtor == 1 && y_dtor == 0);

  {
    preview::variant<X,Y> var{ preview::in_place_index_t<1>{} }; // constructs var(Y)
  }
  EXPECT_TRUE(x_dtor == 1 && y_dtor == 1);
}

template<typename... Vars, typename T, std::enable_if_t<
    preview::detail::variant_assign_check<T, preview::type_sequence<Vars...>>
::value, int> = 0>
bool variant_equal(const preview::variant<Vars...>& var, T&& arg) {
  using Index = preview::detail::variant_overload_type_index<T, preview::type_sequence<Vars...>>;
  const auto* ptr = std::get_if<Index::value>(&var);
  return ptr ? *ptr == std::forward<T>(arg) : false;
}

TEST(VERSIONED(Variant), assignment_operator) {
  preview::variant<int, std::string> a{2017}, b{"CppCon"};
  EXPECT_TRUE((variant_equal(a, 2017)));
  EXPECT_TRUE((variant_equal(b, "CppCon")));

//  (1) operator=( const variant& rhs )
  a = b;
  EXPECT_TRUE((variant_equal(a, "CppCon")));
  EXPECT_TRUE((variant_equal(b, "CppCon")));

//  (2) operator=( variant&& rhs )
  a = std::move(b);
  EXPECT_TRUE((variant_equal(a, "CppCon")));
  EXPECT_TRUE((variant_equal(b, "")));

//  (3) operator=( T&& t ), where T is int
  a = 2019;
  EXPECT_TRUE((variant_equal(a, 2019)));


//    (3) operator=( T&& t ), where T is std::string
  std::string s{"CppNow"};
  a = std::move(s);
  EXPECT_TRUE((variant_equal(a, "CppNow")));
  EXPECT_TRUE(s == "");
}

TEST(VERSIONED(Variant), index) {
  preview::variant<int, std::string> v = "abc";
  EXPECT_EQ(v.index(), 1);
  v = {};
  EXPECT_EQ(v.index(), 0);
}

struct Demo {
  Demo(int) {}
  Demo(const Demo&) { throw std::runtime_error("copy ctor"); }
  Demo& operator= (const Demo&) = default;
};

TEST(VERSIONED(Variant), valueless_by_exception) {
  preview::variant<std::string, Demo> var{"str"};
  EXPECT_EQ(var.index(), 0);
  EXPECT_EQ(std::get<0>(var), "str");
  EXPECT_EQ(var.valueless_by_exception(), false);

  EXPECT_THROW(var = Demo{555}, std::runtime_error);
  EXPECT_EQ(var.index(), preview::variant_npos);
  EXPECT_EQ(var.valueless_by_exception(), true);

  // Now the var is "valueless" which is an invalid state caused
  // by an exception raised in the process of type-changing assignment.

  EXPECT_THROW(std::get<1>(var), preview::bad_variant_access);

  var = "str2";
  EXPECT_EQ(var.index(), 0);
  EXPECT_EQ(std::get<0>(var), "str2");
  EXPECT_EQ(var.valueless_by_exception(), false);
}

TEST(VERSIONED(Variant), emplace) {
  preview::variant<std::string> v1;
  v1.emplace<0>("abc");
  EXPECT_EQ(std::get<0>(v1), "abc");
  v1.emplace<std::string>("def");
  EXPECT_EQ(std::get<0>(v1), "def");

  preview::variant<std::string, std::string> v2;
  v2.emplace<1>("ghi");
  EXPECT_EQ(std::get<1>(v2), "ghi");
}

TEST(VERSIONED(Variant), swap) {
  preview::variant<int, std::string> v1{2}, v2{"abc"};

  std::stringstream ss;
  preview::visit([&](auto&& x) { ss << x; }, v1);
  EXPECT_EQ(ss.str(), "2"); ss.str("");
  preview::visit([&](auto&& x) { ss << x; }, v2);
  EXPECT_EQ(ss.str(), "abc"); ss.str("");

  v1.swap(v2);
  preview::visit([&](auto&& x) { ss << x; }, v1);
  EXPECT_EQ(ss.str(), "abc"); ss.str("");
  preview::visit([&](auto&& x) { ss << x; }, v2);
  EXPECT_EQ(ss.str(), "2"); ss.str("");
}

struct visitor {
  static std::unordered_map<std::type_index, std::function<int&()>>& count_history() {
    static std::unordered_map<std::type_index, std::function<int&()>> history;
    return history;
  }

  template<typename T>
  static int& counter() {
    static int x = 0;
    count_history().emplace(typeid(T), &counter<T>);
    return x;
  }

  static void clear_counter() {
    for (auto& p : count_history()) {
      p.second() = 0;
    }
  }

  template<typename T>
  void operator()(T&&) { ++counter<preview::remove_cvref_t<T>>(); }
};

TEST(VERSIONED(Variant), memberVisit) {
  {
    using var_t = preview::variant<int, long, double, std::string>;

    std::vector<var_t> vec = {10, 15l, 1.5, "hello"};

    auto doubler = [](auto&& arg) -> var_t { return arg + arg; };
    auto out = vec
        | views::transform([&](auto&& v) { return v.visit(doubler); })
        | ranges::to<std::vector>();

    EXPECT_EQ(out.size(), 4);
    EXPECT_TRUE(variant_equal(out[0], 20));
    EXPECT_TRUE(variant_equal(out[1], 30l));
    EXPECT_TRUE(variant_equal(out[2], 3.0));
    EXPECT_TRUE(variant_equal(out[3], "hellohello"));
  }

  {
    visitor::clear_counter();
    preview::variant<int, std::string> var1{42}, var2{"abc"};

    var1.visit(visitor{});
    EXPECT_TRUE(visitor::counter<int>() == 1);

    var2.visit(visitor{});
    EXPECT_TRUE(visitor::counter<std::string>() == 1);
  }
}

struct type_matching_visitor {
  template<typename T>
  static int& counter() {
    static int x = 0;
    return x;
  }

  template<typename T>
  void operator()(T)                  { ++counter<void>(); }
  void operator()(double)             { ++counter<double>(); }
  void operator()(const std::string&) { ++counter<const std::string&>(); }
};

TEST(VERSIONED(Variant), globalVisit) {
  visitor::clear_counter();
  using var_t = preview::variant<int, long, double, std::string>;

  std::vector<var_t> vec = {10, 15l, 1.5, "hello"};

  preview::ranges::for_each(vec, [](auto& v) {
    preview::visit([](auto&& arg) { std::cout << arg; }, v);
  });

  for (auto& v: vec) {
    // 1. void visitor, only called for side-effects (here, for I/O)
    preview::visit([](auto&& arg){ std::cout << arg; }, v);
  }

  // 2. value-returning visitor, demonstrates the idiom of returning another variant
  auto out = vec
      | views::transform([](auto& v) { return preview::visit([](auto&& arg) -> var_t { return arg + arg; }, v); })
      | ranges::to<std::vector>();

  EXPECT_EQ(out.size(), 4);
  EXPECT_TRUE(variant_equal(out[0], 20));
  EXPECT_TRUE(variant_equal(out[1], 30l));
  EXPECT_TRUE(variant_equal(out[2], 3.0));
  EXPECT_TRUE(variant_equal(out[3], "hellohello"));

  for (auto& v: vec) {
    // 3. another type-matching visitor
    preview::visit(type_matching_visitor{}, v);
  }
  EXPECT_EQ(type_matching_visitor::counter<void>(), 2);
  EXPECT_EQ(type_matching_visitor::counter<double>(), 1);
  EXPECT_EQ(type_matching_visitor::counter<const std::string&>(), 1);
}

TEST(VERSIONED(Variant), holds_alternative) {
  preview::variant<int, std::string> v = "abc";
  EXPECT_FALSE(preview::holds_alternative<int>(v));
  EXPECT_TRUE (preview::holds_alternative<std::string>(v));
}

TEST(VERSIONED(Variant), get) {
  preview::variant<int, float> v{12}, w;
  std::cout << std::get<int>(v) << '\n';
  EXPECT_EQ(std::get<int>(v), 12);
  w = std::get<int>(v);
  w = std::get<0>(v); // same effect as the previous line
  EXPECT_EQ(std::get<0>(v), 12);

  EXPECT_FALSE(gettable<double>(v)); // error: no double in [int, float]
  EXPECT_FALSE(gettable<3>(v));      // error: valid index values are 0 and 1

  w = 42.0f;
  EXPECT_EQ(std::get<float>(w), 42.0f);

  w = 42;
  EXPECT_THROW(std::get<float>(w), preview::bad_variant_access);
}

TEST(VERSIONED(Variant), get_if) {
  preview::variant<int, float> v{12}, w{3.f};
  EXPECT_NE(std::get_if<int>(&v), nullptr);
  EXPECT_EQ(std::get_if<int>(&w), nullptr);

  EXPECT_EQ(std::get_if<float>(&v), nullptr);
  EXPECT_NE(std::get_if<float>(&w), nullptr);
}

TEST(VERSIONED(Variant), operator_cmp) {
  preview::variant<int, std::string> v1, v2;

  // Testing operator==
  {
    // by default v1 = 0, v2 = 0;
    EXPECT_TRUE(v1 == v2); // true

    v1 = v2 = 1;
    EXPECT_TRUE(v1 == v2); // true

    v2 = 2;
    EXPECT_FALSE(v1 == v2); // false

    v1 = "A";
    EXPECT_FALSE(v1 == v2); // false: v1.index == 1, v2.index == 0

    v2 = "B";
    EXPECT_FALSE(v1 == v2); // false

    v2 = "A";
    EXPECT_TRUE(v1 == v2); // true
  }

  // Testing operator<
  {
    v1 = v2 = 1;
    EXPECT_FALSE(v1 < v2); // false

    v2 = 2;
    EXPECT_TRUE(v1 < v2); // true

    v1 = 3;
    EXPECT_FALSE(v1 < v2); // false

    v1 = "A"; v2 = 1;
    EXPECT_FALSE(v1 < v2); // false: v1.index == 1, v2.index == 0

    v1 = 1; v2 = "A";
    EXPECT_TRUE(v1 < v2); // true: v1.index == 0, v2.index == 1

    v1 = v2 = "A";
    EXPECT_FALSE(v1 < v2); // false

    v2 = "B";
    EXPECT_TRUE(v1 < v2); // true

    v1 = "C";
    EXPECT_FALSE(v1 < v2); // false
  }

  // Compilation error case: no known conversion
  {
    preview::variant<int, std::string> v3;
    preview::variant<std::string, int> v4;
    EXPECT_FALSE((preview::equality_comparable_with<decltype(v3), decltype(v4)>::value));
  }
}

TEST(VERSIONED(Variant), global_swap) {
  preview::variant<int, std::string> v1{123}, v2{"XYZ"};

  std::swap(v1, v2);
  EXPECT_TRUE(variant_equal(v1, "XYZ"));
  EXPECT_TRUE(variant_equal(v2, 123));

  preview::variant<double, std::string> v3{3.14};
  EXPECT_FALSE((preview::swappable_with<decltype(v1), decltype(v3)>::value));
}

TEST(VERSIONED(Variant), monostate) {
  struct S {
    S(int i) : i(i) {}
    int i;
  };

  // Without the monostate type this declaration will fail.
  // This is because S is not default-constructible.
  preview::variant<preview::monostate, S> var;
  EXPECT_EQ(var.index(), 0);

  EXPECT_THROW(std::get<S>(var), preview::bad_variant_access);

  var = 42;
  EXPECT_EQ(std::get<S>(var).i, 42);
  (void)std::hash<preview::monostate>{}(preview::monostate{});
}

TEST(VERSIONED(Variant), bad_variant_access) {
  preview::variant<int, float> v;
  v = 12;
  EXPECT_THROW(std::get<float>(v), preview::bad_variant_access);
}

TEST(VERSIONED(Variant), variant_size) {
  static_assert(preview::variant_size_v<preview::variant<>> == 0, "");
  static_assert(preview::variant_size_v<preview::variant<int>> == 1, "");
  static_assert(preview::variant_size_v<preview::variant<int, int>> == 2, "");
  static_assert(preview::variant_size_v<preview::variant<int, int, int>> == 3, "");
  static_assert(preview::variant_size_v<preview::variant<int, float, double>> == 3, "");
  static_assert(preview::variant_size_v<preview::variant<preview::monostate, void>> == 2, "");
  static_assert(preview::variant_size_v<preview::variant<const int, const float>> == 2, "");
//  static_assert(preview::variant_size_v<preview::variant<preview::variant<std::any>>> == 1);
}

TEST(VERSIONED(Variant), variant_alternative_t) {
  using my_variant = preview::variant<int, float>;
  static_assert(std::is_same<int,   preview::variant_alternative_t<0, my_variant>>::value, "");
  static_assert(std::is_same<float, preview::variant_alternative_t<1, my_variant>>::value, "");
// cv-qualification on the variant type propagates to the extracted alternative type.
  static_assert(std::is_same<const int, preview::variant_alternative_t<0, const my_variant>>::value, "");
}

TEST(VERSIONED(Variant), hash) {
  using Var = preview::variant<int, int, int, std::string>;
  Var var1{preview::in_place_index<0>, 2020};
  Var var2{preview::in_place_index<1>, 2020};
  EXPECT_NE(std::hash<Var>{}(var1), std::hash<Var>{}(var2));
}
