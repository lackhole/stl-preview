//
// Created by yonggyulee on 2024. 7. 18.
//

#ifndef PREVIEW_ATOMIC_ATOMIC_H_
#define PREVIEW_ATOMIC_ATOMIC_H_

#include <atomic>

#include "boost/atomic.hpp"

namespace preview {

template<typename T>
struct atomic : std::atomic<T> {
 private:
  using std_atomic = std::atomic<T>;

 public:
  using std_atomic::std_atomic;

  void foo() noexcept {

  }

};

} // namespace preview

#endif // PREVIEW_ATOMIC_ATOMIC_H_
