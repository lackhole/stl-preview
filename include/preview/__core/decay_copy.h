//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_CORE_DECAY_COPY_H_
#define PREVIEW_CORE_DECAY_COPY_H_

#include <type_traits>

// Correcting decay_copy requirements of C++20 as if calling auto(x) in C++ 23
// See P0849R8 https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0849r8.html
#define preview_decay_copy(x) std::decay_t<decltype(x)>(x)


#endif // PREVIEW_CORE_DECAY_COPY_H_
