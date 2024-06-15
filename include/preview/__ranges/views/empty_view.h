//
// Created by yonggyulee on 2024/01/02.
//

#ifndef PREVIEW_RANGES_VIEWS_EMPTY_VIEW_H_
#define PREVIEW_RANGES_VIEWS_EMPTY_VIEW_H_

#include <cstddef>
#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__ranges/view_interface.h"

namespace preview {
namespace ranges {

template<typename T>
class empty_view : public ranges::view_interface<empty_view<T>> {
 public:
  static_assert(std::is_object<T>::value, "Constraints not satisfied");

  static constexpr T* begin() noexcept { return nullptr; }

  static constexpr T* end() noexcept { return nullptr; }

  static constexpr T* data() noexcept { return nullptr; }

  static constexpr std::size_t size() noexcept { return 0; }

  static constexpr bool empty() noexcept { return true; }
};

template<typename T>
struct enable_borrowed_range<empty_view<T>> : std::true_type {};

namespace views {

template<typename T>
PREVIEW_INLINE_VARIABLE constexpr empty_view<T> empty{};

} // namespace views

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_EMPTY_VIEW_H_
