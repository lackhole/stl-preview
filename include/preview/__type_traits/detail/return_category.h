//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_TYPE_TRAITS_DETAIL_RETURN_CATEGORY_H_
#define PREVIEW_TYPE_TRAITS_DETAIL_RETURN_CATEGORY_H_

#include <type_traits>

namespace preview {
namespace detail {

template<int v, typename...>
struct return_category : std::integral_constant<int, v> {};

template<int v, typename R>
struct return_category<v, R> : std::integral_constant<int, v> {
  using return_type = R;
};


} // namespace preview
} // namespace detail

#endif // PREVIEW_TYPE_TRAITS_DETAIL_RETURN_CATEGORY_H_
