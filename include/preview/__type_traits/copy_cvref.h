//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_TYPE_TRAITS_COPY_CVREF_H_
#define PREVIEW_TYPE_TRAITS_COPY_CVREF_H_

#include <type_traits>

#include "preview/__type_traits/conditional.h"

namespace preview {

template<typename From, typename To>
using copy_const_t = std::conditional_t<std::is_const<From>::value, std::add_const_t<To>, To>;

template<typename From, typename To>
using copy_volatile_t = std::conditional_t<std::is_volatile<From>::value, std::add_volatile_t<To>, To>;

template<typename From, typename To>
using copy_cv_t = copy_const_t<From, copy_volatile_t<From, To>>;

template<typename From, typename To>
using copy_reference_t = conditional_t<
    std::is_rvalue_reference<From>, std::add_rvalue_reference_t<To>,
    std::is_lvalue_reference<From>, std::add_lvalue_reference_t<To>,
    To
>;

template<typename From, typename To>
using copy_cvref_t = copy_reference_t<From, copy_cv_t<From, To>>;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_COPY_CVREF_H_
