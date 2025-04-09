//
// Created by yonggyulee on 24/03/2025
//

#ifndef PREVIEW_ALGORITHM_RANGES_MOVE_RESULT_H_
#define PREVIEW_ALGORITHM_RANGES_MOVE_RESULT_H_

#include "preview/__algorithm/ranges/in_out_result.h"

namespace preview {
namespace ranges {

template<typename I, typename O>
using move_result = in_out_result<I, O>;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_MOVE_RESULT_H_
