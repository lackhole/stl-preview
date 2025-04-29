//
// Created by yonggyulee on 28/04/2025
//

#ifndef PREVIEW_RANGES_VIEWS_AS_RVALUE_VIEW_H_
#define PREVIEW_RANGES_VIEWS_AS_RVALUE_VIEW_H_

#include <iterator>
#include <type_traits>
#include <utility>

#include "preview/__core/cxx_version.h"

#if PREVIEW_CXX_VERSION >= 20
#include <concepts>
#endif

#include "preview/__concepts/copy_constructible.h"
#include "preview/__iterator/move_sentinel.h"
#include "preview/__ranges/approximately_sized_range.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/common_range.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/simple_view.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__ranges/views/all.h"

namespace preview {
namespace ranges {

template<typename V>
class as_rvalue_view : public view_interface<as_rvalue_view<V>> {
 public:
  static_assert(view<V>::value, "Constraints not satisfied");
  static_assert(input_range<V>::value, "Constraints not satisfied");

  as_rvalue_view() = default;

  constexpr explicit as_rvalue_view(V base)
      : base_(std::move(base)) {}

  template<bool B = copy_constructible<V>::value, std::enable_if_t<B, int> = 0>
  constexpr V base() const & { return base_; }
  constexpr V base() && { return std::move(base_); }

  template<bool B = !simple_view<V>::value, std::enable_if_t<B, int> = 0>
  constexpr auto begin() {
    return std::move_iterator<decltype(ranges::begin(base_))>(ranges::begin(base_));
  }
  template<bool B = range<const V>::value, std::enable_if_t<B, int> = 0>
  constexpr auto begin() const {
    return std::move_iterator<decltype(ranges::begin(base_))>(ranges::begin(base_));
  }

  template<bool B = !simple_view<V>::value, std::enable_if_t<B, int> = 0>
  constexpr auto end() {
    return end_impl(common_range<V>{});
  }
  template<bool B = range<const V>::value, std::enable_if_t<B, int> = 0>
  constexpr auto end() const {
    return end_impl(common_range<const V>{});
  }

  template<bool B = sized_range<V>::value, std::enable_if_t<B, int> = 0>
  constexpr auto size() {
    return ranges::size(base_);
  }
  template<bool B = sized_range<const V>::value, std::enable_if_t<B, int> = 0>
  constexpr auto size() const {
    return ranges::size(base_);
  }

  template<bool B = approximately_sized_range<V>::value, std::enable_if_t<B, int> = 0>
  constexpr auto reserve_hint() {
    return ranges::reserve_hint(base_);
  }
  template<bool B = approximately_sized_range<const V>::value, std::enable_if_t<B, int> = 0>
  constexpr auto reserve_hint() const {
    return ranges::reserve_hint(base_);
  }

 private:
  constexpr auto end_impl(std::true_type /* common_range */) {
    return std::move_iterator<decltype(ranges::end(base_))>(ranges::end(base_));
  }
  constexpr auto end_impl(std::false_type /* common_range */) {
    return move_sentinel<decltype(ranges::end(base_))>(ranges::end(base_));
  }
  constexpr auto end_impl(std::true_type /* common_range */) const {
    return std::move_iterator<decltype(ranges::end(base_))>(ranges::end(base_));
  }
  constexpr auto end_impl(std::false_type /* common_range */) const {
    return move_sentinel<decltype(ranges::end(base_))>(ranges::end(base_));
  }

  V base_ = V();
};

#if PREVIEW_CXX_VERSION >= 17
template<typename R>
as_rvalue_view(R&&) -> as_rvalue_view<views::all_t<R>>;
#endif

} // namespace ranges
} // namespace preview 

#endif // PREVIEW_RANGES_VIEWS_AS_RVALUE_VIEW_H_
