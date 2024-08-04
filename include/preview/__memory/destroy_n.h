//
// Created by yonggyulee on 2024. 8. 4.
//

#ifndef PREVIEW_MEMORY_DESTROY_N_H_
#define PREVIEW_MEMORY_DESTROY_N_H_

#include "preview/__memory/addressof.h"
#include "preview/__memory/destroy_at.h"

namespace preview {

template<typename NothrowForwardIterator, typename Size>
constexpr NothrowForwardIterator destroy_n(NothrowForwardIterator first, Size n) {
  for (; n > 0; (void)++first, --n)
    preview::destroy_at(preview::addressof(*first));
  return first;
}

} // namespace preview

#endif // PREVIEW_MEMORY_DESTROY_N_H_
