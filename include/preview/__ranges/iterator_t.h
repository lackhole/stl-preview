//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_RANGES_ITERATOR_T_H_
#define PREVIEW_RANGES_ITERATOR_T_H_

#include <type_traits>

#include "preview/__ranges/begin.h"
#include "preview/__type_traits/is_referenceable.h"

namespace preview {
namespace ranges {

template<typename T, bool = is_referencable<T>::value, typename = void>
struct iterator {};

template<typename T>
struct iterator<T, true, void_t<decltype( ranges::begin(std::declval<T&>()) )>> {
  using type = decltype(ranges::begin(std::declval<T&>()));
};

template<typename T>
using iterator_t = typename ranges::iterator<T>::type;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_ITERATOR_T_H_
