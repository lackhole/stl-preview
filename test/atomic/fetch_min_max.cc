//
// Created by YongGyuLee on 2025-03-02.
//

#include "preview/atomic.h"

#include <atomic>
#include <random>
#include <thread>
#include <vector>

#include "../test_utils.h"

#ifdef TYPED_TEST_SUITE

template<typename T>
class FetchMinMax : public testing::Test {};
using Types = testing::Types<
    int,
    unsigned,
    long,
    unsigned long,
    long long,
    unsigned long long,
    std::intptr_t*
>;
TYPED_TEST_SUITE(FetchMinMax, Types, CVQualifiedNameGenerator);

VERSIONED_TYPED_TEST(FetchMinMax, fetch_max) {
  preview::atomic<TypeParam> x;
  std::mt19937 gen{std::random_device{}()};
  std::uniform_int_distribution<int> dist{-5, 5};

  std::vector<std::thread> threads(10);
  for (auto& thread : threads) {
    thread = std::thread([&]() {
      for (int i = 0; i < 100'000; ++i) {
        auto prev = x.load(std::memory_order_relaxed);
        auto next = prev + dist(gen);
        auto value = x.fetch_max(next, std::memory_order_relaxed);
        ASSERT_GE(value, prev);
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
  std::cout << "x = " << x << '\n';
}


VERSIONED_TYPED_TEST(FetchMinMax, fetch_min) {
  preview::atomic<TypeParam> x;
  std::mt19937 gen{std::random_device{}()};
  std::uniform_int_distribution<int> dist{-5, 5};

  std::vector<std::thread> threads(10);
  for (auto& thread : threads) {
    thread = std::thread([&]() {
      for (int i = 0; i < 100'000; ++i) {
        auto prev = x.load(std::memory_order_relaxed);
        auto next = prev + dist(gen);
        auto value = x.fetch_min(next, std::memory_order_relaxed);
        ASSERT_LE(value, prev);
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
  std::cout << "x = " << x << '\n';
}

#endif
