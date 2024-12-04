#include "preview/bit.h"
#include "gtest.h"

#include <cstdint>

union U {
  int i;
  char c[sizeof(int)];
};

TEST(VERSIONED(bit), bit_cast) {
  U u{10};
  EXPECT_EQ(preview::bit_cast<int>(u.c), 10);

  constexpr double f64v = 19880124.0;
  auto u64v = preview::bit_cast<std::uint64_t>(f64v);
  EXPECT_EQ(preview::bit_cast<double>(u64v), f64v); // round-trip

  constexpr std::uint64_t u64v2 = 0x3fe9000000000000ull;
  auto f64v2 = preview::bit_cast<double>(u64v2);
  EXPECT_EQ(preview::bit_cast<std::uint64_t>(f64v2), u64v2); // round-trip
}

TEST(VERSIONED(bit), byteswap_unsigned) {
  constexpr std::uint8_t byte{ 0xCD };
  EXPECT_EQ(preview::byteswap<std::uint8_t>(byte), byte);

  constexpr std::uint16_t word{ 0xCA'FE };
  EXPECT_EQ(preview::byteswap<std::uint16_t>(word), 0xFE'CA);
  constexpr std::uint16_t word_same{ 0x44'44 };
  EXPECT_EQ(preview::byteswap<std::uint16_t>(word_same), word_same);

  constexpr std::uint32_t dword{ 0xDE'AD'BE'EF };
  EXPECT_EQ(preview::byteswap<std::uint32_t>(dword), 0xEF'BE'AD'DE);
  constexpr std::uint32_t dword_same{ 0x44'44'44'44 };
  EXPECT_EQ(preview::byteswap<std::uint32_t>(dword_same), dword_same);

  constexpr std::uint64_t qword{ 0x01'23'45'67'89'AB'CD'EF };
  EXPECT_EQ(preview::byteswap<std::uint64_t>(qword), 0xEF'CD'AB'89'67'45'23'01);
  constexpr std::uint64_t qword_same{ 0x44'44'44'44'44'44'44'44 };
  EXPECT_EQ(preview::byteswap<std::uint64_t>(qword_same), qword_same);
}

TEST(VERSIONED(bit), byteswap_signed) {
  constexpr std::int8_t byte{ 0x7D };
  EXPECT_EQ(preview::byteswap<std::int8_t>(byte), byte);

  constexpr std::int16_t word{ 0x7A'7E };
  EXPECT_EQ(preview::byteswap<std::int16_t>(word), 0x7E'7A);
  constexpr std::int16_t word_same{ 0x44'44 };
  EXPECT_EQ(preview::byteswap<std::int16_t>(word_same), word_same);

  constexpr std::int32_t dword{ 0x7E'AD'BE'7F };
  EXPECT_EQ(preview::byteswap<std::int32_t>(dword), 0x7F'BE'AD'7E);
  constexpr std::int32_t dword_same{ 0x44'44'44'44 };
  EXPECT_EQ(preview::byteswap<std::int32_t>(dword_same), dword_same);

  constexpr std::int64_t qword{ 0x01'23'45'67'89'AB'CD'44 };
  EXPECT_EQ(preview::byteswap<std::int64_t>(qword), 0x44'CD'AB'89'67'45'23'01);
  constexpr std::int64_t qword_same{ 0x44'44'44'44'44'44'44'44 };
  EXPECT_EQ(preview::byteswap<std::int64_t>(qword_same), qword_same);
}

#if PREVIEW_CXX >= 17
  #define IF_CONSTEXPR if constexpr
#else
  #define IF_CONSTEXPR if
#endif

TEST(VERSIONED(bit), endianness) {
  const std::uint32_t n{ 0x01'23'45'67 };
  const std::uint8_t* const ptr{ reinterpret_cast<const std::uint8_t*>(&n) };

  IF_CONSTEXPR (preview::endian::native == preview::endian::little) {
    EXPECT_EQ(ptr[0], 0x67);
    EXPECT_EQ(ptr[1], 0x45);
    EXPECT_EQ(ptr[2], 0x23);
    EXPECT_EQ(ptr[3], 0x01);
  } else IF_CONSTEXPR (preview::endian::native == preview::endian::big) {
    EXPECT_EQ(ptr[0], 0x01);
    EXPECT_EQ(ptr[1], 0x23);
    EXPECT_EQ(ptr[2], 0x45);
    EXPECT_EQ(ptr[3], 0x67);
  }
}