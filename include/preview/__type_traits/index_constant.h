//
// Created by lux on 2024. 6. 15..
//

#ifndef PREVIEW_TYPE_TRAITS_INDEX_CONSTANT_H_
#define PREVIEW_TYPE_TRAITS_INDEX_CONSTANT_H_

#include <type_traits>

namespace preview {

template<std::size_t v>
using size_constant = std::integral_constant<std::size_t, v>;

template<std::size_t v>
using index_constant = size_constant<v>;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_INDEX_CONSTANT_H_
