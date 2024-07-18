//
// Created by yonggyulee on 2024. 7. 18.
//

#ifndef PREVIEW_INCLUDE_PREVIEW___ATOMIC_ATOMIC_H_
#define PREVIEW_INCLUDE_PREVIEW___ATOMIC_ATOMIC_H_

#include <atomic>

namespace preview {

template<typename T>
struct atomic : std::atomic<T> {
 private:
  using std_atomic = std::atomic<T>;

 public:
  using std_atomic::std_atomic;

};

} // namespace preview

#endif // PREVIEW_INCLUDE_PREVIEW___ATOMIC_ATOMIC_H_
