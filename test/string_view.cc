#include "preview/string_view.h"

#include "test_utils.h"

#include <bitset>
#include <string>

#if PREVIEW_CXX_VERSION >= 17
#include <string_view>
#include <fstream>
#endif

#if !defined(__ANDROID__)
#include <filesystem>
#endif

using namespace std::literals;
using namespace preview::literals;

#if PREVIEW_CXX_VERSION >= 17

struct templated_std_basic_string_view_fn {
  template<typename CharT, typename Traits>
  constexpr void operator()(std::basic_string_view<CharT, Traits>) const {}
};

TEST(VERSIONED(StringView), STLConversion_TemplatedBasicStringView) {
  EXPECT_TRUE_TYPE(preview::is_invocable<templated_std_basic_string_view_fn, preview::string_view>);
}

#if !defined(__ANDROID__)

TEST(VERSIONED(StringView), STLConversion_Path) {
  EXPECT_EQ(
      (std::is_constructible_v<std::filesystem::path, std::string_view>),
      (std::is_constructible_v<std::filesystem::path, preview::string_view>));
  EXPECT_EQ(
      (std::is_convertible_v<std::filesystem::path, std::string_view>),
      (std::is_convertible_v<std::filesystem::path, preview::string_view>));
}

#endif

TEST(VERSIONED(StringView), STLConversion_FStream) {
  EXPECT_EQ(
      (std::is_constructible_v<std::fstream, std::string_view>),
      (std::is_constructible_v<std::fstream, preview::string_view>));
  EXPECT_EQ(
      (std::is_convertible_v<std::fstream, std::string_view>),
      (std::is_convertible_v<std::fstream, preview::string_view>));
}

struct BothConstructible {
  template<typename CharT, typename Traits>
  BothConstructible(std::basic_string_view<CharT, Traits>) {}
  template<typename CharT, typename Traits>
  BothConstructible(preview::basic_string_view<CharT, Traits>) {}
};

TEST(VERSIONED(StringView), STLConversion_Ambiguity) {
  BothConstructible(std::string_view{});
  BothConstructible(preview::string_view{});
}

#endif


// ---------- C++26 ----------

TEST(VERSIONED(StringView), P2591R5) {
  preview::string_view sv = "ABC";
  std::string s = "DEF";

  EXPECT_EQ_TYPE(decltype(sv + s), std::string);
  EXPECT_EQ_TYPE(decltype(sv + std::move(s)), std::string);

  EXPECT_EQ(sv + s, "ABCDEF"s);
  EXPECT_EQ(s + sv, "DEFABC"s);
  EXPECT_EQ(sv + std::move(s), "ABCDEF"s);
  EXPECT_EQ(std::move(s) + sv, "ABC"s);
}

TEST(VERSIONED(StringView), P2697R1) {
  std::bitset<8> b("110010"_sv);
  EXPECT_EQ(b.to_string(), "00110010");

  EXPECT_EQ(std::bitset<10>("11010011"_sv), std::bitset<10>("11010011"s));
  EXPECT_EQ(std::bitset<10>("00000"_sv), std::bitset<10>("00000"s));
  EXPECT_EQ(std::bitset<10>("11111"_sv), std::bitset<10>("11111"s));
}

TEST(VERSIONED(StringView), P2495R3) {
  EXPECT_EQ(std::stringstream{"12345"s}.str(), std::stringstream{"12345"_sv}.str());
  EXPECT_EQ(std::istringstream{"12345"s}.str(), std::istringstream{"12345"_sv}.str());
  EXPECT_EQ(std::ostringstream{"12345"s}.str(), std::ostringstream{"12345"_sv}.str());
  EXPECT_EQ(std::stringbuf{"12345"s}.str(), std::stringbuf{"12345"_sv}.str());
}
