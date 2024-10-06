#include "preview/any.h"
#include "gtest.h"

#include <complex>
#include <functional>
#include <iomanip>
#include <set>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "preview/algorithm.h"
#include "preview/string_view.h"

using namespace std::literals;
using namespace preview::literals;

TEST(VERSIONED(Any), basics) {
  // any type
  preview::any a = 1;
  EXPECT_STREQ(a.type().name(), typeid(int).name());
  EXPECT_EQ(preview::any_cast<int>(a), 1);

  a = 3.14;
  EXPECT_STREQ(a.type().name(), typeid(double).name());
  EXPECT_EQ(preview::any_cast<double>(a), 3.14);

  a = true;
  EXPECT_STREQ(a.type().name(), typeid(bool).name());
  EXPECT_EQ(preview::any_cast<bool>(a), true);

  // bad cast
  EXPECT_THROW(preview::any_cast<float>(a), preview::bad_any_cast);

  // has value
  a = 2;
  EXPECT_TRUE(a.has_value());
  EXPECT_FALSE(preview::any().has_value());

  // reset
  a.reset();
  EXPECT_FALSE(a.has_value());

  // pointer to contained data
  a = 3;
  int* i = preview::any_cast<int>(&a);
  EXPECT_EQ(*i, 3);
}

struct A
{
  int age;
  std::string name;
  double salary;

  A(int age, std::string name, double salary)
      : age(age), name(std::move(name)), salary(salary) {}
};

TEST(VERSIONED(Any), constructor) {
  { // Constructor #1: default constructor
    preview::any a;
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(a.type(), typeid(void));
  }

  { // Constructor #2: copy constructor
    preview::any a = 5;
    preview::any b = a;
    EXPECT_EQ(preview::any_cast<int>(b), 5);
  }

  { // Constructor #3: move constructor
    preview::any a = "hello"s;
    preview::any b = std::move(a);
    EXPECT_EQ(preview::any_cast<std::string>(b), "hello"s);
  }

  { // Constructor #4: preview::any holding int
    preview::any a{7};
    EXPECT_EQ(a.type(), typeid(int));
  }

  { // Constructor #5: preview::any holding A, constructed in place
    preview::any a(preview::in_place_type<A>, 30, "Ada", 1000.25);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(preview::any_cast<A>(a).age, 30);
    EXPECT_EQ(preview::any_cast<A>(a).name, "Ada");
    EXPECT_EQ(preview::any_cast<A>(a).salary, 1000.25);
  }

  { // Constructor #6: preview::any holding a set of A with custom comparison
    auto lambda = [](auto&& l, auto&& r) { return l.age < r.age; };
    preview::any a3(
        preview::in_place_type<std::set<A, decltype(lambda)>>,
        {
            A{39, "Ada"s, 100.25},
            A{20, "Bob"s, 75.5}
        },
        lambda);

    using set_type = std::set<A, decltype(lambda)>;

    EXPECT_EQ(a3.type(), typeid(set_type));
    EXPECT_EQ(preview::any_cast<set_type>(a3).size(), 2);
    EXPECT_EQ(preview::any_cast<set_type>(a3).begin()->age, 20);
    EXPECT_EQ(preview::any_cast<set_type>(a3).begin()->name, "Bob");
    EXPECT_EQ(preview::any_cast<set_type>(a3).begin()->salary, 75.5);
    EXPECT_EQ(preview::any_cast<set_type>(a3).rbegin()->age, 39);
    EXPECT_EQ(preview::any_cast<set_type>(a3).rbegin()->name, "Ada");
    EXPECT_EQ(preview::any_cast<set_type>(a3).rbegin()->salary, 100.25);
  }
}

struct ThrowOnCopy {
  ThrowOnCopy() = default;
  ThrowOnCopy(const ThrowOnCopy&) { throw std::runtime_error(""); }
  ThrowOnCopy(ThrowOnCopy&&) = default;
};

TEST(VERSIONED(Any), operator_assign) {
  std::string cat{"cat"};

  preview::any a1{42};
  preview::any a2{cat};
  EXPECT_EQ(a1.type(), typeid(int));
  EXPECT_EQ(a2.type(), typeid(std::string));

  a1 = a2; // overload (1)
  EXPECT_EQ(a1.type(), typeid(std::string));
  EXPECT_EQ(a2.type(), typeid(std::string));
  EXPECT_EQ(preview::any_cast<std::string&>(a1), cat);
  EXPECT_EQ(preview::any_cast<std::string&>(a2), cat);

  a1 = 96; // overload (3)
  a2 = "dog"s; // overload (3)
  a1 = std::move(a2); // overload (2)
  EXPECT_EQ(a1.type(), typeid(std::string));
  EXPECT_EQ(preview::any_cast<std::string&>(a1), "dog");
  // The state of a2 is valid but unspecified. In fact,
  // it is void in gcc/clang and std::string in msvc.
  std::cout << "a2.type(): " << std::quoted(a2.type().name()) << '\n';

  a1 = std::move(cat); // overload (3)
  EXPECT_EQ(*preview::any_cast<std::string>(&a1), "cat");
  // The state of cat is valid but indeterminate:
  std::cout << "cat: " << std::quoted(cat) << '\n';

  preview::any a3 = ThrowOnCopy{};
  try {
    a1 = a3;
    FAIL();
  } catch (const std::exception& e) {
    EXPECT_NE(dynamic_cast<const std::runtime_error*>(&e), nullptr);
    EXPECT_TRUE(a1.has_value()) << "operator= must satisfy strong exception safety guarantee";
    EXPECT_EQ(preview::any_cast<std::string>(a1), "cat");
  }
}

struct Star {
  Star(std::string name, int id)
      : name{name}
      , id{id} {}

  std::string name;
  int id;
};

TEST(VERSIONED(Any), emplace) {
  {
    preview::any celestial;
    // (1) emplace(Args&&... args);
    celestial.emplace<Star>("Procyon", 2943);
    const auto* star = preview::any_cast<Star>(&celestial);
    EXPECT_NE(star, nullptr);
    EXPECT_EQ(star->name, "Procyon");
    EXPECT_EQ(star->id, 2943);
  }

  {
    preview::any av;
    // (2) emplace(std::initializer_list<U> il, Args&&... args);
    av.emplace<std::vector<char>>({'C', '+', '+', '1', '7'} /* no args */);
    EXPECT_EQ(av.type(), typeid(std::vector<char>));
    const auto* va = preview::any_cast<std::vector<char>>(&av);
    EXPECT_TRUE(preview::ranges::equal(*va, "C++17"_sv));
  }
}

TEST(VERSIONED(Any), reset) {
  preview::any a{42};
  EXPECT_TRUE(a.has_value());
  a.reset();
  EXPECT_FALSE(a.has_value());
}

TEST(VERSIONED(Any), swap) {
  preview::any a, b;
  EXPECT_FALSE(a.has_value());
  EXPECT_FALSE(b.has_value());

  a.swap(b);
  EXPECT_FALSE(a.has_value());
  EXPECT_FALSE(b.has_value());

  a = 10;
  a.swap(b);
  EXPECT_FALSE(a.has_value());
  EXPECT_TRUE (b.has_value());

  b.swap(a);
  EXPECT_TRUE (a.has_value());
  EXPECT_FALSE(b.has_value());

  b = 20;
  swap(a, b);
  EXPECT_EQ(preview::any_cast<int>(a), 20);
  EXPECT_EQ(preview::any_cast<int>(b), 10);
}

template<typename T, typename F>
struct any_visitor_invoker {
  const F f;

  void operator()(const preview::any& a) const {
    call(a, std::is_void<T>{});
  }

 private:
  void call(const preview::any&, std::true_type) const { f(); }
  void call(const preview::any& a, std::false_type) const { f(preview::any_cast<T const&>(a)); }
};

template<class T, class F>
std::pair<const std::type_index, std::function<void(preview::any const&)>> to_any_visitor(F const& f) {
  return {std::type_index(typeid(T)), any_visitor_invoker<T, F>{f}};
}

std::stringstream& static_stream() {
  static auto str = new std::stringstream();
  return *str;
}

using any_visitor_type = std::unordered_map<std::type_index, std::function<void(preview::any const&)>>;
any_visitor_type& any_visitor() {
  static auto inst = new any_visitor_type {
    to_any_visitor<void>([] { static_stream() << "{}"; }),
    to_any_visitor<int>([](int x) { static_stream() << x; }),
    to_any_visitor<unsigned>([](unsigned x) { static_stream() << x; }),
    to_any_visitor<float>([](float x) { static_stream() << x; }),
    to_any_visitor<double>([](double x) { static_stream() << x; }),
    to_any_visitor<char const*>([](char const* s) { static_stream() << std::quoted(s); }),
    // ... add more handlers for your types ...
  };

  return *inst;
}

void process(const preview::any& a) {
  const auto it = any_visitor().find(std::type_index(a.type()));
  if (it != any_visitor().cend())
    it->second(a);
  else
    static_stream() << "Unregistered type "<< std::quoted(a.type().name());
}

template<class T, class F>
void register_any_visitor(F const& f) {
  any_visitor().insert(to_any_visitor<T>(f));
}

TEST(VERSIONED(Any), type) {
  std::vector<preview::any> va{{}, 42, 123u, 3.14159f, 2.71828, "C++17"};

  for (const preview::any& a : va) {
    process(a);
    static_stream() << ", ";
  }

  std::stringstream ss;
  ss << "{}"         << ", "
     << 42           << ", "
     << 123u         << ", "
     << 3.14159f     << ", "
     << 2.71828      << ", "
     << "\"C++17\""  << ", ";

  EXPECT_EQ(static_stream().str(), ss.str());
  static_stream().str("");

  process(preview::any(0xFULL));
  EXPECT_TRUE(preview::ranges::contains_subrange(static_stream().str(), "Unregistered"_sv));
  static_stream().str("");

  register_any_visitor<unsigned long long>([](auto x) {
      static_stream() << std::hex << std::showbase << x;
  });

  process(preview::any(0xFULL)); //< OK: 0xf
  EXPECT_EQ(static_stream().str(), "0xf");
}

TEST(VERSIONED(Any), make_any) {
  auto a0 = preview::make_any<std::string>("Hello, preview::any!\n");
  auto a1 = preview::make_any<std::complex<double>>(0.1, 2.3);

  using lambda = std::function<void(void)>;

  // Put a lambda into preview::any. Attempt #1 (failed).
  preview::any a2 = [] { std::cout << "Lambda #1.\n"; };
  EXPECT_NE(a2.type(), typeid(lambda));

  // any_cast casts to <void(void)> but actual type is not
  // a std::function..., but ~ main::{lambda()#1}, and it is
  // unique for each lambda. So, this throws...
  EXPECT_THROW(preview::any_cast<lambda>(a2)(), preview::bad_any_cast);

  // Put a lambda into preview::any. Attempt #2 (successful).
  auto a3 = preview::make_any<lambda>([] { std::cout << "Lambda #2.\n"; });
  EXPECT_EQ(a3.type(), typeid(lambda));
  EXPECT_NO_THROW(preview::any_cast<lambda>(a3)());
}

TEST(VERSIONED(Any), any_cast) {
  {
    preview::any x(5);                              // x holds int
    EXPECT_EQ(preview::any_cast<int>(x), 5);        // cast to value
    preview::any_cast<int&>(x) = 10;                // cast to reference
    EXPECT_EQ(preview::any_cast<int>(x), 10);

    x = "Meow";                                     // x holds const char*
    EXPECT_STREQ(preview::any_cast<const char*>(x), "Meow");
    preview::any_cast<const char*&>(x) = "Harry";
    EXPECT_STREQ(preview::any_cast<const char*>(x), "Harry");

    x = "Meow"s;                                            // x holds std::string
    std::string s, s2("Jane");
    s = std::move(preview::any_cast<std::string&>(x));     // move from preview::any
    EXPECT_EQ(s, "Meow");
    preview::any_cast<std::string&>(x) = std::move(s2);    // move to preview::any
    EXPECT_EQ(preview::any_cast<const std::string&>(x), "Jane");

    std::string cat("Meow");
    const preview::any y(cat);                             // const y holds std::string
    EXPECT_EQ(preview::any_cast<const std::string&>(y), cat);

//  preview::any_cast<std::string&>(y);
  }

  {
    // Simple example
    auto a1 = preview::any(12);

    EXPECT_THROW(preview::any_cast<std::string>(a1), preview::bad_any_cast);

    // Pointer example
    EXPECT_NE(preview::any_cast<int>(&a1), nullptr);
    EXPECT_EQ(preview::any_cast<std::string>(&a1), nullptr);

    // Advanced example
    a1 = std::string("hello");
    auto& ra = preview::any_cast<std::string&>(a1); //< reference
    ra[1] = 'o';
    EXPECT_EQ(preview::any_cast<std::string>(a1), "hollo");
    EXPECT_EQ(preview::any_cast<const std::string&>(a1), "hollo"); //< const reference

    auto s1 = preview::any_cast<std::string&&>(std::move(a1)); //< rvalue reference
    // Note: “s1” is a move-constructed std::string:
    static_assert(std::is_same<decltype(s1), std::string>::value, "");

    // Note: the std::string in “a1” is left in valid but unspecified state
    EXPECT_EQ(a1.has_value(), true);
    EXPECT_EQ(preview::any_cast<std::string>(&a1)->size(), 0);
    EXPECT_EQ(s1.size(), 5);
  }
}
