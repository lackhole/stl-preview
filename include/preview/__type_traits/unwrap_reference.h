//
// Created by YongGyu Lee on 2024. 7. 9.
//

#ifndef PREVIEW_TYPE_TRAITS_UNWRAP_REFERENCE_H_
#define PREVIEW_TYPE_TRAITS_UNWRAP_REFERENCE_H_

#include <functional>
#include <type_traits>

#include "preview/__functional/reference_wrapper.h"

namespace preview {

template<typename T>
struct unwrap_reference {
  using type = T;
};

template<typename T>
struct unwrap_reference<std::reference_wrapper<T>> {
  using type = T&;
};

template<typename T>
struct unwrap_reference<preview::reference_wrapper<T>> {
  using type = T&;
};

template<typename T>
using unwrap_reference_t = typename unwrap_reference<T>::type;

template<typename T>
struct unwrap_ref_decay : unwrap_reference<std::decay_t<T>> {};

template<typename T>
using unwrap_ref_decay_t = typename unwrap_ref_decay<T>::type;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_UNWRAP_REFERENCE_H_
