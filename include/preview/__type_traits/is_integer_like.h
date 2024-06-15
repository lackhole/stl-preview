//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_TYPE_TRAITS_IS_INTEGER_LIKE_H_
#define PREVIEW_TYPE_TRAITS_IS_INTEGER_LIKE_H_

#include <limits>
#include <type_traits>

#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"

namespace preview {

template<typename T>
struct is_integer_like
    : disjunction<
        std::is_integral<T>,
        conjunction<
          bool_constant<std::numeric_limits<T>::is_integer>,
          bool_constant<std::numeric_limits<T>::is_specialized>
        >
      > {};

template<typename T>
struct is_signed_integer_like
    : disjunction<
        conjunction<
          std::is_integral<T>,
          std::is_signed<T>
        >,
        conjunction<
          bool_constant<std::numeric_limits<T>::is_integer>,
          bool_constant<std::numeric_limits<T>::is_signed>,
          bool_constant<std::numeric_limits<T>::is_specialized>
        >
      > {};

template<typename T>
struct is_unsigned_integer_like
    : disjunction<
        conjunction<
          std::is_integral<T>,
          std::is_unsigned<T>
        >,
        conjunction<
          bool_constant<std::numeric_limits<T>::is_integer>,
          bool_constant<!std::numeric_limits<T>::is_signed>,
          bool_constant<std::numeric_limits<T>::is_specialized>
        >
      > {};

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_IS_INTEGER_LIKE_H_
