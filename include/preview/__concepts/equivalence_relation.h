//
// Created by yonggyulee on 2023/12/27.
//

#ifndef PREVIEW_CONCEPTS_EQUIVALENCE_RELATION_H_
#define PREVIEW_CONCEPTS_EQUIVALENCE_RELATION_H_

#include "preview/__concepts/relation.h"

namespace preview {

template<typename R, typename T, typename U>
struct equivalence_relation : relation<R, T, U> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_EQUIVALENCE_RELATION_H_
