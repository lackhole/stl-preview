//
// Created by yonggyulee on 2024/01/02.
//

#ifndef PREVIEW_RANGES_VIEWS_SINGLE_H_
#define PREVIEW_RANGES_VIEWS_SINGLE_H_

#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__concepts/constructible_from.h"
#include "preview/__concepts/copy_constructible.h"
#include "preview/__ranges/movable_box.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__utility/in_place.h"

namespace preview {
namespace ranges {

template<typename T>
class single_view : public ranges::view_interface<single_view<T>> {
 public:
  static_assert(copy_constructible<T>::value, "Constraints not satisfied");
  static_assert(std::is_object<T>::value, "Constraints not satisfied");

  constexpr single_view() = default;

  constexpr explicit single_view(const T& t)
      : value_(t) {}

  constexpr explicit single_view(T&& t)
      : value_(std::move(t)) {}

  template<typename InPlace, typename... Args, std::enable_if_t<conjunction<
      std::is_same<InPlace, preview::in_place_t>,
      constructible_from<T, Args...>
  >::value, int> = 0>
  constexpr explicit single_view(InPlace, Args&&... args)
      : value_{preview::in_place, std::forward<Args>(args)...} {}

  constexpr T* begin() noexcept { return data(); }
  constexpr const T* begin() const noexcept { return data(); }

  constexpr T* end() noexcept { return data() + 1; }
  constexpr const T* end() const noexcept { return data() + 1; }

  static constexpr std::size_t size() noexcept { return 1; }

  constexpr T* data() noexcept { return value_.operator->(); }
  constexpr const T* data() const noexcept { return value_.operator->(); }

 private:
  movable_box<T> value_;
};

#if __cplusplus >= 201703L

template<typename T>
single_view(T) -> single_view<T>;

#endif

namespace views {
namespace detail {

struct single_niebloid {
  template<typename T, std::enable_if_t<conjunction<
      copy_constructible<std::decay_t<T>>,
      std::is_object<std::decay_t<T>>,
      std::is_constructible<std::decay_t<T>, T&&>
  >::value, int> = 0>
  constexpr single_view<std::decay_t<T>>
  operator()(T&& t) const {
    return single_view<std::decay_t<T>>(std::forward<T>(t));
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::single_niebloid single{};

} // namespace niebloid
using namespace niebloid;

} // namespace preview
} // namespace ranges
} // namespace views

#endif // PREVIEW_RANGES_VIEWS_SINGLE_H_
