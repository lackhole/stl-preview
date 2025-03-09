//
// Created by YongGyuLee on 2025-03-02.
//

#include "preview/atomic.h"

#include <memory>
#include <thread>
#include <vector>

#include "../test_utils.h"

TEST(VERSIONED(Atomic), atomic_flag_ctor) {
  preview::atomic_flag f;
  EXPECT_EQ(f.test(), false);
}

TEST(VERSIONED(Atomic), atomic_flag_test) {
  preview::atomic_flag f;
  EXPECT_EQ(f.test_and_set(), false);
  EXPECT_EQ(f.test(), true);
  f.clear();
  EXPECT_EQ(f.test(), false);
}

TEST(VERSIONED(Atomic), atomic_flag_wait_notify) {
  preview::atomic_flag f;
  preview::atomic<int> wakeup;
  std::vector<std::thread> threads(5);
  for (auto& thread : threads) {
    thread = std::thread([&] {
      f.wait(true, std::memory_order_relaxed);
      wakeup.fetch_add(1, std::memory_order_relaxed);
    });
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  f.test_and_set();
  f.notify_one();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_GT(wakeup, 0);
  EXPECT_LE(wakeup, 5);

  f.notify_all();
  for (auto& thread : threads) {
    thread.join();
  }
  EXPECT_EQ(wakeup, 5);
}
