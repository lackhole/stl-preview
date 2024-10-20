//
// Created by yonggyulee on 2024. 10. 17.
//

#ifndef PREVIEW_FUNCTIONAL_IS_REFERENCE_WRAPPER_H_
#define PREVIEW_FUNCTIONAL_IS_REFERENCE_WRAPPER_H_

#include <functional>
#include <type_traits>

namespace preview {

template<typename T>
struct is_reference_wrapper : std::false_type {};

template<typename T>
struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type {};

} // namespace preview

#endif // PREVIEW_FUNCTIONAL_IS_REFERENCE_WRAPPER_H_
