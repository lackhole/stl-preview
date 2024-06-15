//
// Created by yonggyulee on 2024/01/04.
//

#ifndef PREVIEW_RANGES_SUBRANGE_KIND_H_
#define PREVIEW_RANGES_SUBRANGE_KIND_H_

namespace preview {
namespace ranges {

enum
#if !defined(_MSC_VER) || __cplusplus >= 201703L
class
#endif
subrange_kind : bool {
    unsized,
    sized
};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_SUBRANGE_KIND_H_
