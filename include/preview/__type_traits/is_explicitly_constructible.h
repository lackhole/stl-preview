//
// Created by YongGyu Lee on 11/13/23.
//

#ifndef PREVIEW_TYPE_TRAITS_IS_EXPLICITLY_CONSTRUCTIBLE_H_
#define PREVIEW_TYPE_TRAITS_IS_EXPLICITLY_CONSTRUCTIBLE_H_

#include <type_traits>

#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/negation.h"

namespace preview {

template<typename T, typename... Args>
struct is_explicitly_constructible :
    conjunction<
      std::is_constructible<T, Args...>,
      negation<std::is_convertible<Args..., T>>
    > {};

template<typename T, typename... Args>
using is_explicitly_constructible_t = typename is_explicitly_constructible<T, Args...>::type;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_IS_EXPLICITLY_CONSTRUCTIBLE_H_
