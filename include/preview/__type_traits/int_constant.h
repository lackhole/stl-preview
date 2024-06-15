#ifndef PREVIEW_TYPE_TRAITS_INT_CONSTANT_H_
#define PREVIEW_TYPE_TRAITS_INT_CONSTANT_H_

#include <type_traits>

namespace preview {

template<int v>
using int_constant = std::integral_constant<int, v>;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_INT_CONSTANT_H_
