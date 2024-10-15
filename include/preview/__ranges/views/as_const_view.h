//
// Created by yonggyulee on 1/15/24.
//

#ifndef PREVIEW_RANGES_VIEWS_AS_CONST_VIEW_H_
#define PREVIEW_RANGES_VIEWS_AS_CONST_VIEW_H_

#include <utility>

#include "preview/__concepts/copy_constructible.h"
#include "preview/__concepts/default_initializable.h"
#include "preview/__ranges/cbegin.h"
#include "preview/__ranges/simple_view.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__ranges/view.h"

namespace preview {
namespace ranges {

template<typename V>
class as_const_view : public view_interface<as_const_view<V>> {
 public:
  static_assert(view<V>::value, "Constraints not satisfied");
  static_assert(input_range<V>::value, "Constraints not satisfied");

  template<bool B = default_initializable<V>::value, std::enable_if_t<B, int> = 0>
  constexpr as_const_view() {}

  constexpr explicit as_const_view(V base)
      : base_(std::move(base)) {}

  template<typename V2 = V, std::enable_if_t<copy_constructible<V2>::value, int> = 0>
  constexpr V base() const& {
    return base_;
  }

  constexpr V base() && {
    return std::move(base_);
  }

  template<typename V2 = V, std::enable_if_t<simple_view<V2>::value == false, int> = 0>
  constexpr auto begin() {
    return preview::ranges::cbegin(base_);
  }

  template<typename V2 = const V, std::enable_if_t<range<V2>::value, int> = 0>
  constexpr auto begin() const {
    return preview::ranges::cbegin(base_);
  }

  template<typename V2 = V, std::enable_if_t<simple_view<V2>::value == false, int> = 0>
  constexpr auto end() {
    return preview::ranges::cend(base_);
  }

  template<typename V2 = const V, std::enable_if_t<range<V2>::value, int> = 0>
  constexpr auto end() const {
    return preview::ranges::cend(base_);
  }

  template<typename V2 = V, std::enable_if_t<sized_range<V2>::value, int> = 0>
  constexpr auto size() {
    return preview::ranges::size(base_);
  }

  template<typename V2 = const V, std::enable_if_t<sized_range<V2>::value, int> = 0>
  constexpr auto size() const {
    return preview::ranges::size(base_);
  }

 private:
  V base_{};
};

#if __cplusplus >= 201703L

template<typename R>
as_const_view(R&&) -> as_const_view<views::all_t<R>>;

#endif

} // namespace ranges
} // namespace preview

template<typename T>
PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(preview::ranges::as_const_view<T>)
    = preview::ranges::enable_borrowed_range<T>;

#endif // PREVIEW_RANGES_VIEWS_AS_CONST_VIEW_H_
