//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_CONCEPTS_SEMIREGULAR_H_
#define PREVIEW_CONCEPTS_SEMIREGULAR_H_

#include "preview/__concepts/copyable.h"
#include "preview/__concepts/default_initializable.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {

template<typename T>
struct semiregular
    : conjunction<
        copyable<T>,
        default_initializable<T>
      > {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_SEMIREGULAR_H_
