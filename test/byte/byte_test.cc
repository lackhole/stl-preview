//
// Created by YongGyu Lee on 2024. 6. 15..
//

#include "preview/byte.h"
#include "preview/__utility/to_underlying.h"

#include <bitset>

#include "../test_utils.h"

static std::bitset<8> ToBit(preview::byte b) {
  return preview::to_integer<int>(b);
}

TEST(VERSIONED(byte), basics) {
  // std::byte y = 1; // Error: cannot convert int to byte.
  preview::byte y{1}; // OK

  // if (y == 13) {} // Error: cannot be compared.
  if (y == preview::byte{13}) {} // OK, bytes are comparable


  preview::byte b{42};
  EXPECT_EQ(ToBit(b), 0b00101010);

  // b *= 2; // Error: b is not of arithmetic type
  b <<= 1;
  EXPECT_EQ(ToBit(b), 0b01010100);

  b >>= 1;
  EXPECT_EQ(ToBit(b), 0b00101010);

  EXPECT_EQ(ToBit(b << 1), 0b01010100);
  EXPECT_EQ(ToBit(b >> 1), 0b00010101);


  b |= preview::byte{0b11110000};
  EXPECT_EQ(ToBit(b), 0b11111010);

  b &= preview::byte{0b11110000};
  EXPECT_EQ(ToBit(b), 0b11110000);

  b ^= preview::byte{0b11111111};
  EXPECT_EQ(ToBit(b), 0b00001111);
}


TEST(VERSIONED(byte), to_integer) {
  preview::byte y{1};

  int arr[] = {1, 2, 3};
  (void)arr[preview::to_integer<int>(y)];
}

TEST(VERSIONED(byte), to_underlying) {
  preview::byte y{1};

  int arr[] = {1, 2, 3};
  (void)arr[preview::to_underlying(y)];
}
