//
// Created by yonggyulee on 2024. 9. 2.
//

#ifndef PREVIEW_ATOMIC_DETAIL_ATOMIC_GENERIC_H_
#define PREVIEW_ATOMIC_DETAIL_ATOMIC_GENERIC_H_

#include "preview/__atomic/detail/cxx20_atomic_base.h"

namespace preview {
namespace detail {

template<typename T>
struct atomic_generic : public cxx20_atomic_base<T> {};

} // namespace detail
} // namespace preview

#endif // PREVIEW_ATOMIC_DETAIL_ATOMIC_GENERIC_H_
