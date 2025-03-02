//
// Created by YongGyuLee on 2025-03-02.
//

#include "preview/atomic.h"

#include <memory>

#include "../test_utils.h"

TEST(VERSIONED(Atomic), fetch_add_float) {
  preview::atomic<float> f1{0.0f};

  EXPECT_EQ(f1.fetch_add(1.0f), 0.0f);
  EXPECT_EQ(f1.fetch_add(0.000001f), 0.0f + 1.0f);
  EXPECT_EQ(f1.fetch_add(123456789.0f), 0.0f + 1.0f + 0.000001f);
  EXPECT_EQ(f1.fetch_add(0.0f), 0.0f + 1.0f + 0.000001f + 123456789.0f);
}

TEST(VERSIONED(Atomic), fetch_add_double) {
  preview::atomic<double> f1{0.0};
  EXPECT_EQ(f1.fetch_add(1.0), 0.0);
  EXPECT_EQ(f1.fetch_add(0.000001), 0. + 1.0);
  EXPECT_EQ(f1.fetch_add(123456789.0), 0.0 + 1.0 + 0.000001);
  EXPECT_EQ(f1.fetch_add(0.0), 0.0 + 1.0 + 0.000001 + 123456789.0);
}
