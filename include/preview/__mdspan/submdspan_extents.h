//
// Created by yonggyulee on 2024. 10. 26.
//

#ifndef PREVIEW_MDSPAN_SUBMDSPAN_EXTENTS_H_
#define PREVIEW_MDSPAN_SUBMDSPAN_EXTENTS_H_

#include <cstddef>
#include <type_traits>

#include "preview/__mdspan/extents.h"

namespace preview {

//template<typename IndexType, std::size_t... Extents, typename... SliceSpecifiers, std::enable_if_t<
//    (sizeof...(SliceSpecifiers) == extents<IndexType, Extents...>::rank()), int> = 0>
//constexpr auto submdspan_extents(const extents<IndexType, Extents...>& src, SliceSpecifiers... slices) {
//
//}

} // namespace preview

#endif // PREVIEW_MDSPAN_SUBMDSPAN_EXTENTS_H_
