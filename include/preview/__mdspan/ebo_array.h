//
// Created by yonggyulee on 18/11/2024
//

#ifndef PREVIEW_MDSPAN_EBO_ARRAY_H_
#define PREVIEW_MDSPAN_EBO_ARRAY_H_

#include <array>

namespace preview {
namespace detail {

template<typename T>
struct empty_array {
  constexpr T& operator[](std::size_t) noexcept { return std::declval<T&>(); }
  constexpr const T& operator[](std::size_t) const noexcept { return std::declval<const T&>(); }
};

} // namespace detail
} // namespace preview

#endif // PREVIEW_MDSPAN_EBO_ARRAY_H_
