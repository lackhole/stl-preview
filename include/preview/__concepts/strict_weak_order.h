//
// Created by yonggyulee on 2023/12/27.
//

#ifndef PREVIEW_CONCEPTS_STRICT_WEAK_ORDER_H_
#define PREVIEW_CONCEPTS_STRICT_WEAK_ORDER_H_

#include "preview/__concepts/relation.h"

namespace preview {

template<typename R, typename T, typename U>
struct strict_weak_order : relation<R, T, U> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_STRICT_WEAK_ORDER_H_
