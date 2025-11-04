//
// Created by yonggyulee on 2/8/24.
//

#ifndef PREVIEW_RANGES_VIEWS_REVERSE_VIEW_H_
#define PREVIEW_RANGES_VIEWS_REVERSE_VIEW_H_

#include <iterator>
#include <type_traits>
#include <utility>

#include "preview/__concepts/copy_constructible.h"
#include "preview/__concepts/default_initializable.h"
#include "preview/__iterator/next.h"
#include "preview/__ranges/approximately_sized_range.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/bidirectional_range.h"
#include "preview/__ranges/common_range.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/non_propagating_cache.h"
#include "preview/__ranges/reserve_hint.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/views/all.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename V, bool = common_range<V>::value /* true; no cache */>
struct reverse_view_cached_begin {
  constexpr std::reverse_iterator<iterator_t<V>> begin(V& base) {
    return std::make_reverse_iterator(ranges::end(base));
  }
};

template<typename V>
struct reverse_view_cached_begin<V, false> {
  constexpr std::reverse_iterator<iterator_t<V>> begin(V& base) {
    if (!cached_begin_.has_value()) {
      cached_begin_.emplace(std::make_reverse_iterator(ranges::next(ranges::begin(base), ranges::end(base))));
    }
    return cached_begin_.value();
  }

  non_propagating_cache<std::reverse_iterator<iterator_t<V>>> cached_begin_;
};

} // namespace detail

template<typename V>
class reverse_view
    : public view_interface<reverse_view<V>>
    , private detail::reverse_view_cached_begin<V>
{
  using cache_begin_base = detail::reverse_view_cached_begin<V>;
 public:
  static_assert(view<V>::value, "Constraints not satisfied");
  static_assert(bidirectional_range<V>::value, "Constraints not satisfied");

  template<bool B = default_initializable<V>::value, std::enable_if_t<B, int> = 0>
  constexpr reverse_view() {}

  constexpr reverse_view(V r)
      : base_(std::move(r)) {}

  template<typename V2 = V, std::enable_if_t<copy_constructible<V2>::value, int> = 0>
  constexpr V base() const& {
    return base_;
  }

  constexpr V base() && {
    return std::move(base_);
  }

  constexpr std::reverse_iterator<iterator_t<V>> begin() {
    return cache_begin_base::begin(base_);
  }

  template<typename V2 = V, std::enable_if_t<common_range<const V2>::value, int> = 0>
  constexpr auto begin() const {
    return std::make_reverse_iterator(ranges::end(base_));
  }

  constexpr std::reverse_iterator<iterator_t<V>> end() {
    return std::make_reverse_iterator(ranges::begin(base_));
  }

  template<typename V2 = V, std::enable_if_t<common_range<const V2>::value, int> = 0>
  constexpr auto end() const {
    return std::make_reverse_iterator(ranges::begin(base_));
  }

  template<typename V2 = V, std::enable_if_t<sized_range<V2>::value, int> = 0>
  constexpr auto size() {
    return ranges::size(base_);
  }

  template<typename V2 = V, std::enable_if_t<sized_range<const V2>::value, int> = 0>
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
  V base_{};
};

#if __cplusplus >= 201703L

template<typename R>
reverse_view(R&&) -> reverse_view<views::all_t<R>>;

#endif

template<typename R>
constexpr reverse_view<views::all_t<R>> make_reverse_view(R&& r) {
  return reverse_view<views::all_t<R>>(std::forward<R>(r));
}

} // namespace ranges
} // namespace preview

template<typename T>
PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(preview::ranges::reverse_view<T>)
    = preview::ranges::enable_borrowed_range<T>;

#endif // PREVIEW_RANGES_VIEWS_REVERSE_VIEW_H_
