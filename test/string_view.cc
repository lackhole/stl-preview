#include "preview/string_view.h"

#include "test_utils.h"

#include <bitset>
#include <string>
#include <sstream>

using namespace std::literals;
using namespace preview::literals;



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
