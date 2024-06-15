//
// Created by yonggyulee on 1/23/24.
//

#ifndef PREVIEW_SPAN___FORWARD_DECLARE_H_
#define PREVIEW_SPAN___FORWARD_DECLARE_H_

#include <cstddef>

#include "preview/__span/dynamic_extent.h"

namespace preview {

template<typename T, std::size_t Extent = dynamic_extent>
class span;

} // namespace preview

#endif // PREVIEW_SPAN___FORWARD_DECLARE_H_
