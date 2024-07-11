#include "preview/utility.h"
#include "gtest.h"

#include <complex>
#include <memory>
#include <vector>

#include "preview/optional.h"

using namespace std::literals;

static constexpr int kTypeLvalueReference = 0x0;
static constexpr int kTypeRvalueReference = 0x1;
static constexpr int kTypeConst = 0x2;

struct TypeTeller
{
  int operator()() const &  { return kTypeLvalueReference | kTypeConst; }
  int operator()() &        { return kTypeLvalueReference;              }
  int operator()() const && { return kTypeRvalueReference | kTypeConst; }
  int operator()() &&       { return kTypeRvalueReference;              }
};

struct FarStates {
  std::unique_ptr<TypeTeller> ptr;
  preview::optional<TypeTeller> opt;
  std::vector<TypeTeller> container;

  template<typename Self>
  static auto&& from_ptr_impl(Self&& self) {
    if (!self.ptr)
      throw preview::bad_optional_access{};
    return preview::forward_like<decltype(self)>(*self.ptr);
    // It is not good to use *std::forward<decltype(self)>(self).ptr, because
    // std::unique_ptr<TypeTeller> always dereferences to a non-const lvalue.
  }

  template<typename Self>
  static auto&& from_index_impl(Self&& self, std::size_t i) {
    return preview::forward_like<decltype(self)>(self.container.at(i));
    // It is not so good to use std::forward<decltype(self)>(self)[i], because
    // containers do not provide rvalue subscript access, although they could.
  }

  template<typename Self>
  static auto&& from_opt_impl(Self&& self) {
    return preview::forward_like<decltype(self)>(self.opt.value());
    // It is OK to use std::forward<decltype(self)>(self).opt.value(),
    // because preview::optional provides suitable accessors.
  }

  auto&& from_opt() const &  { return from_opt_impl(*this); }
  auto&& from_opt()       &  { return from_opt_impl(*this); }
  auto&& from_opt() const && { return from_opt_impl(std::move(*this)); }
  auto&& from_opt()       && { return from_opt_impl(std::move(*this)); }

  auto&& operator[](std::size_t i) const &  { return from_index_impl(*this, i); }
  auto&& operator[](std::size_t i)       &  { return from_index_impl(*this, i); }
  auto&& operator[](std::size_t i) const && { return from_index_impl(std::move(*this), i); }
  auto&& operator[](std::size_t i)       && { return from_index_impl(std::move(*this), i); }

  auto&& from_ptr() const &  { return from_ptr_impl(*this); }
  auto&& from_ptr()       &  { return from_ptr_impl(*this); }
  auto&& from_ptr() const && { return from_ptr_impl(std::move(*this)); }
  auto&& from_ptr()       && { return from_ptr_impl(std::move(*this)); }
};

struct empty {};
struct not_empty {
  int x;
};

TEST(VERSIONED(Utility), compressed_pair) {
  {
    EXPECT_EQ(sizeof(preview::compressed_pair<int      , int  >), sizeof(int) * 2);
    EXPECT_EQ(sizeof(preview::compressed_pair<empty    , int  >), sizeof(int)    );
    EXPECT_EQ(sizeof(preview::compressed_pair<int      , empty>), sizeof(int)    );
    EXPECT_EQ(sizeof(preview::compressed_pair<not_empty, int  >), sizeof(int) * 2);
    EXPECT_EQ(sizeof(preview::compressed_pair<empty    , empty>), 1);
  }

  { // ctor
    preview::compressed_pair<int, float> p1;
    EXPECT_EQ(p1.first(), 0);
    EXPECT_EQ(p1.second(), 0.f);

    preview::compressed_pair<int, double> p2{42, 3.1415};
    EXPECT_EQ(p2.first(), 42);
    EXPECT_EQ(p2.second(), 3.1415);

    preview::compressed_pair<char, int> p4{p2};
    EXPECT_EQ(p4.first(), '*');
    EXPECT_EQ(p4.second(), 3);

    preview::compressed_pair<std::complex<double>, std::string> p6
        { std::piecewise_construct, std::forward_as_tuple(0.123, 7.7), std::forward_as_tuple(10, 'a') };
    EXPECT_EQ(p6.first().real(), 0.123);
    EXPECT_EQ(p6.first().imag(), 7.7);
    EXPECT_EQ(p6.second(), "aaaaaaaaaa"s);

  }
}
