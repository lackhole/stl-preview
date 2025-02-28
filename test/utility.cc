#include "preview/utility.h"
#include "test_utils.h"

#include <memory>
#include <vector>

#include "preview/optional.h"

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

TEST(VERSIONED(Utility), forward_like) {
  FarStates my_state;
  my_state.ptr = std::make_unique<TypeTeller>();
  my_state.opt.emplace(TypeTeller{});
  my_state.container = std::vector<TypeTeller>(1);

  EXPECT_EQ(my_state.from_ptr()(), kTypeLvalueReference);
  EXPECT_EQ(my_state.from_opt()(), kTypeLvalueReference);
  EXPECT_EQ(my_state[0](),         kTypeLvalueReference);

  EXPECT_EQ(preview::as_const(my_state).from_ptr()(), kTypeLvalueReference | kTypeConst);
  EXPECT_EQ(preview::as_const(my_state).from_opt()(), kTypeLvalueReference | kTypeConst);
  EXPECT_EQ(preview::as_const(my_state)[0](),         kTypeLvalueReference | kTypeConst);

  EXPECT_EQ(std::move(my_state).from_ptr()(), kTypeRvalueReference);
  EXPECT_EQ(std::move(my_state).from_opt()(), kTypeRvalueReference);
  EXPECT_EQ(std::move(my_state)[0](),         kTypeRvalueReference);

  EXPECT_EQ(std::move(preview::as_const(my_state)).from_ptr()(), kTypeRvalueReference | kTypeConst);
  EXPECT_EQ(std::move(preview::as_const(my_state)).from_opt()(), kTypeRvalueReference | kTypeConst);
  EXPECT_EQ(std::move(preview::as_const(my_state))[0](),         kTypeRvalueReference | kTypeConst);
}

TEST(VERSIONED(Utility), force_forward_like) {
  FarStates my_state;
  my_state.ptr = std::make_unique<TypeTeller>();
  my_state.opt.emplace(TypeTeller{});
  my_state.container = std::vector<TypeTeller>(1);

  const auto& cref_my_state = my_state;

  EXPECT_EQ(cref_my_state.from_ptr()(), kTypeLvalueReference | kTypeConst);
  EXPECT_EQ(cref_my_state.from_opt()(), kTypeLvalueReference | kTypeConst);
  EXPECT_EQ(cref_my_state[0](),         kTypeLvalueReference | kTypeConst);

  EXPECT_EQ(preview::force_forward_like<decltype((my_state))>(cref_my_state.from_ptr())(), kTypeLvalueReference);
  EXPECT_EQ(preview::force_forward_like<decltype((my_state))>(cref_my_state.from_opt())(), kTypeLvalueReference);
  EXPECT_EQ(preview::force_forward_like<decltype((my_state))>(cref_my_state[0])(),         kTypeLvalueReference);
}

#ifdef GTEST_HAS_DEATH_TEST
TEST(VERSIONED(Utility), unreachable) {
//#ifndef NDEBUG
//  EXPECT_DEATH(preview::unreachable(), "");
//#endif
}
#endif
