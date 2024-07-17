//
// Created by yonggyulee on 2024. 7. 14..
//

#ifndef PREVIEW_CONCEPTS_INTEGER_LIKE_H_
#define PREVIEW_CONCEPTS_INTEGER_LIKE_H_

#include <type_traits>

#include "preview/__concepts/integral.h"
#include "preview/__concepts/integer_class.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/negation.h"

namespace preview {
namespace detail {

template<typename T>
struct integer_like_unwrap_cv :
    disjunction<
        conjunction<
            negation<std::is_same<T, bool>>,
            integral<T>
        >,
        integer_class<T>
    > {};

} // namespace detail

// https://eel.is/c++draft/iterator.concept.winc#4
template<typename T>
struct integer_like : detail::integer_like_unwrap_cv<std::remove_cv_t<T>> {};

template<typename T>
struct signed_integer_like :
    conjunction<
        integer_like<T>,
        disjunction<
            signed_integral<T>,
            signed_integer_class<T>
        >
    > {};

template<typename T>
struct unsigned_integer_like :
    conjunction<
        integer_like<T>,
        disjunction<
            unsigned_integral<T>,
            unsigned_integer_class<T>
        >
    > {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_INTEGER_LIKE_H_
