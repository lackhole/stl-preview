//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_CONCEPTS_SAME_AS_H_
#define PREVIEW_CONCEPTS_SAME_AS_H_

#include <type_traits>

namespace preview {

template<typename T, typename U>
struct same_as : std::is_same<T, U> {};

} // namespace preview

#endif // PREVIEW_CONCEPTS_SAME_AS_H_
