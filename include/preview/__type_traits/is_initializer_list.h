//
// Created by cosge on 2023-12-29.
//

#ifndef PREVIEW_TYPE_TRAITS_IS_INITIALIZER_LIST_H_
#define PREVIEW_TYPE_TRAITS_IS_INITIALIZER_LIST_H_

#include <initializer_list>
#include <type_traits>

#include "preview/__type_traits/is_specialization.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {

template <class T>
struct is_initializer_list : is_specialization<remove_cvref_t<T>, std::initializer_list> {};

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_IS_INITIALIZER_LIST_H_
