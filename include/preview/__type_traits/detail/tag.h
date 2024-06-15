//
// Created by yonggyulee on 2023/12/26.
//

#ifndef PREVIEW_TYPE_TRAITS_DETAIL_TAG_H_
#define PREVIEW_TYPE_TRAITS_DETAIL_TAG_H_

#include <type_traits>

namespace preview {
namespace detail {

template<int v>
using tag_v = std::integral_constant<int, v>;

// 0 is used for ELSE, which indicates false. All valid tags should start from 1
using tag_else = tag_v<0>;

using tag_1 = tag_v<1>;
using tag_2 = tag_v<2>;
using tag_3 = tag_v<3>;
using tag_4 = tag_v<4>;
using tag_5 = tag_v<5>;
using tag_6 = tag_v<6>;
using tag_7 = tag_v<7>;
using tag_8 = tag_v<8>;
using tag_9 = tag_v<9>;

template<int v, typename If, typename... ElseIf>
struct conditional_tag_impl : std::conditional_t<bool(If::value), tag_v<v>, conditional_tag_impl<v + 1, ElseIf...>> {};

template<int v, typename If>
struct conditional_tag_impl<v, If> : std::conditional_t<bool(If::value), tag_v<v>, tag_else> {};

template<typename If, typename... ElseIf>
struct conditional_tag : conditional_tag_impl<1, If, ElseIf...> {};

} // namespace detail
} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_DETAIL_TAG_H_
