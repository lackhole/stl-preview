//
// Created by yonggyulee on 2/3/24.
//

#ifndef PREVIEW_RANGES_VIEWS_COMMON_VIEW_H_
#define PREVIEW_RANGES_VIEWS_COMMON_VIEW_H_

#include <type_traits>
#include <utility>

#include "preview/__concepts/copyable.h"
#include "preview/__iterator/common_iterator.h"
#include "preview/__ranges/borrowed_range.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/common_range.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/random_access_range.h"
#include "preview/__ranges/size.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__ranges/views/all.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/negation.h"

namespace preview {
namespace ranges {

template<typename V>
class common_view : public view_interface<common_view<V>> {
 public:
  static_assert(common_range<V>::value == false, "Constraints not satisfied");
  static_assert(copyable<iterator_t<V>>::value, "Constraints not satisfied");

  common_view() = default;

  constexpr explicit common_view(V r)
      : base_(std::move(r)) {}

  template<typename V2 = V, std::enable_if_t<copy_constructible<V2>::value, int> = 0>
  constexpr V base() const& {
    return base_;
  }

  constexpr V base() && {
    return std::move(base_);
  }

  constexpr auto begin() {
    return begin_impl<V>(base_, conjunction<random_access_range<V>, sized_range<V>>{});
  }

  template<typename V2 = V, std::enable_if_t<range<const V2>::value, int> = 0>
  constexpr auto begin() const {
    return begin_impl<const V2>(base_, conjunction<random_access_range<const V2>, sized_range<const V2>>{});
  }

  constexpr auto end() {
    return end_impl<V>(base_, conjunction<random_access_range<V>, sized_range<V>>{});
  }

  template<typename V2 = V, std::enable_if_t<range<const V2>::value, int> = 0>
  constexpr auto end() const {
    return end_impl<const V>(base_, conjunction<random_access_range<const V>, sized_range<const V>>{});
  }

  template<typename V2 = V, std::enable_if_t<sized_range<V2>::value, int> = 0>
  constexpr auto size() {
    return ranges::size(base_);
  }

  template<typename V2 = V, std::enable_if_t<sized_range<const V2>::value, int> = 0>
  constexpr auto size() const {
    return ranges::size(base_);
  }

 private:
  template<typename V2, typename Base>
  static constexpr auto begin_impl(Base&& base, std::true_type /* random_and_sized */) {
    return ranges::begin(base);
  }
  template<typename V2, typename Base>
  static constexpr auto begin_impl(Base&& base, std::false_type /* random_and_sized */) {
    return common_iterator<iterator_t<V2>, sentinel_t<V2>>(ranges::begin(base));
  }

  template<typename V2, typename Base>
  static constexpr auto end_impl(Base&& base, std::true_type /* random_and_sized */) {
    return ranges::begin(base) + ranges::size(base);
  }
  template<typename V2, typename Base>
  static constexpr auto end_impl(Base&& base, std::false_type /* random_and_sized */) {
    return common_iterator<iterator_t<V2>, sentinel_t<V2>>(ranges::end(base));
  }

  V base_{};
};

#if __cplusplus >= 201703L

template<typename R>
common_view(R&&) -> common_view<views::all_t<R>>;

#endif

template<typename R, std::enable_if_t<viewable_range<R>::value, int> = 0>
constexpr common_view<views::all_t<R>> make_common_view(R&& r) {
  return common_view<views::all_t<R>>(std::forward<R>(r));
}

} // namespace ranges
} // namespace preview

template<typename V>
PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(preview::ranges::common_view<V>)
    = preview::ranges::enable_borrowed_range<V>;

#endif // PREVIEW_RANGES_VIEWS_COMMON_VIEW_H_
