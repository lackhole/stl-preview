//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_CONCEPTS_DESTRUCTIBLE_H_
#define PREVIEW_CONCEPTS_DESTRUCTIBLE_H_

#include <type_traits>

namespace preview {

template<typename T>
struct destructible : std::is_nothrow_destructible<T> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_DESTRUCTIBLE_H_
