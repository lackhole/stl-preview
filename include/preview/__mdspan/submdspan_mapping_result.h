//
// Created by yonggyulee on 2024. 9. 27.
//

#ifndef PREVIEW_MDSPAN_SUBMDSPAN_SLICE_RESULT_H_
#define PREVIEW_MDSPAN_SUBMDSPAN_SLICE_RESULT_H_

#include "preview/__core/no_unique_address.h"

namespace preview {

template<typename LayoutMapping>
struct submdspan_mapping_result {
  PREVIEW_NO_UNIQUE_ADDRESS LayoutMapping mapping = LayoutMapping();
  std::size_t offset{};
};

} // namespace preview

#endif // PREVIEW_MDSPAN_SUBMDSPAN_SLICE_RESULT_H_
