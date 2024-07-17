//
// Created by yonggyulee on 2024. 7. 14..
//

#ifndef PREVIEW_CONCEPTS_INTEGER_CLASS_H_
#define PREVIEW_CONCEPTS_INTEGER_CLASS_H_

#include <limits>
#include <type_traits>

#include "preview/__concepts/integral.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/is_explicitly_convertible.h"
#include "preview/__type_traits/negation.h"

namespace preview {

// https://eel.is/c++draft/iterator.concept.winc#2
template<typename T>
struct integer_class :
    conjunction<
        bool_constant<std::numeric_limits<T>::is_integer>,
        bool_constant<std::numeric_limits<T>::is_specialized>,
        is_explicitly_convertible<T, int>,
        std::is_nothrow_constructible<T, int>,
        std::is_nothrow_constructible<T, T>
    > {};

template<typename T>
struct signed_integer_class :
    conjunction<
        integer_class<T>,
        bool_constant<std::numeric_limits<T>::is_signed>
    > {};

template<typename T>
struct unsigned_integer_class :
    conjunction<
        integer_class<T>,
        negation<bool_constant<std::numeric_limits<T>::is_signed>>
    > {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_INTEGER_CLASS_H_
