//
// Created by yonggyulee on 2024. 9. 29.
//

#ifndef PREVIEW_MDSPAN_INDEX_PAIR_LIKE_H_
#define PREVIEW_MDSPAN_INDEX_PAIR_LIKE_H_

#include <tuple>
#include <type_traits>

#include "preview/__concepts/convertible_to.h"
#include "preview/__tuple/tuple_like.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace detail {

// TODO: Use concept

template<typename T, typename IndexType, bool = /* false */ pair_like<T>::value>
struct index_pair_like : std::false_type {};

template<typename T, typename IndexType>
struct index_pair_like<T, IndexType, true>
    : conjunction<
        convertible_to<std::tuple_element_t<0, T>, IndexType>,
        convertible_to<std::tuple_element_t<1, T>, IndexType>
    > {};

} // namespace detail
} // namespace preview

#endif // PREVIEW_MDSPAN_INDEX_PAIR_LIKE_H_
