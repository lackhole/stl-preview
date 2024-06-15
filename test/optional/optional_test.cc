//
// Created by YongGyu Lee on 2024. 6. 15..
//

#include "preview/optional.h"

#include "../gtest.h"

TEST(VERSIONED(optional), basics) {

}

struct NoMove {
  NoMove() = default;
  NoMove(const NoMove&) {
    copied += 1;
  }
  NoMove(NoMove&&) = delete;
  NoMove& operator=(const NoMove&) = default;
  static int copied;
};
int NoMove::copied{};

TEST(VERSIONED(optional), copy_over_move) {
  preview::optional<NoMove> op1{preview::in_place};
  preview::optional<NoMove> op2;
  std::swap(op1, op2); // copy is selected for non-movable object
  EXPECT_EQ(NoMove::copied, 2);
}
