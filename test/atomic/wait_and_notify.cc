//
// Created by YongGyuLee on 2025-03-02.
//

#include "preview/atomic.h"

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

#include "../test_utils.h"

TEST(VERSIONED(Atomic), wait_notify) {
  preview::atomic<bool> all_tasks_completed{false};
  preview::atomic<unsigned> completion_count{};
  std::future<void> task_futures[10];
  preview::atomic<unsigned> outstanding_task_count{10};

  // Spawn several tasks which take different amounts of
  // time, then decrement the outstanding task count.
  for (std::future<void>& task_future : task_futures) {
    task_future = std::async([&] {
        // This sleep represents doing real work...
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        ++completion_count;
        --outstanding_task_count;

        // When the task count falls to zero, notify
        // the waiter (main thread in this case).
        if (outstanding_task_count.load() == 0) {
          all_tasks_completed = true;
          all_tasks_completed.notify_one();
        }
    });
  }

  (void)all_tasks_completed.wait(false);

  std::cout << "Tasks completed = " << completion_count.load() << '\n';
  SUCCEED();
}
