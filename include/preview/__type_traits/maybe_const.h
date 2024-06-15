//
// Created by YongGyu Lee on 1/26/24.
//

#ifndef PREVIEW_TYPE_TRAITS_MAYBE_CONST_H_
#define PREVIEW_TYPE_TRAITS_MAYBE_CONST_H_

#include <type_traits>

namespace preview {

template<bool Const, typename V>
using maybe_const = std::conditional_t<Const, const V, V>;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_MAYBE_CONST_H_
