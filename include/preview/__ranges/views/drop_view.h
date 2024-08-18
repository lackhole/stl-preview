//
// Created by yonggyulee on 2024/01/28.
//

#ifndef PREVIEW_RANGES_VIEWS_DROP_VIEW_H_
#define PREVIEW_RANGES_VIEWS_DROP_VIEW_H_

#include <algorithm>
#include <type_traits>
#include <utility>

#include "preview/__concepts/copy_constructible.h"
#include "preview/__iterator/next.h"
#include "preview/__ranges/simple_view.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/non_propagating_cache.h"
#include "preview/__ranges/random_access_range.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/range_difference_t.h"
#include "preview/__ranges/size.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/views/all.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename DropView, typename V,
    bool Cached = negation<conjunction<
        simple_view<V>,
        sized_range<const V>,
        random_access_range<const V>
    >>::value /* true */>
class drop_view_cached_begin : public view_interface<DropView> {
 public:
  // Not a random access range. Cache required
  constexpr auto begin(V& base, range_difference_t<V> count) {
    if (!begin_.has_value()) {
      begin_.emplace(ranges::next(ranges::begin(base), count, ranges::end(base)));
    }
    return *begin_;
  }

 private:
  non_propagating_cache<iterator_t<V>> begin_;
};

template<typename DropView, typename V>
class drop_view_cached_begin<DropView, V, false> : public view_interface<DropView> {
 public:
  // Random access range. No cache required
  constexpr auto begin(V& base, range_difference_t<V> count) {
    return ranges::next(ranges::begin(base), count, ranges::end(base));
  }
};

} // namespace detail

template<typename V>
class drop_view : public detail::drop_view_cached_begin<drop_view<V>, V> {
  using begin_base = detail::drop_view_cached_begin<drop_view<V>, V>;

 public:
  static_assert(view<V>::value, "Constraints not satisfied");

  drop_view() = default;

  constexpr explicit drop_view(V base, range_difference_t<V> count)
      : base_(std::move(base)), count_(count) {}


  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      copy_constructible<V>
  >::value, int> = 0>
  constexpr V base() const& noexcept(std::is_nothrow_copy_constructible<V>::value) {
    return base_;
  }

  constexpr V base() && noexcept(std::is_nothrow_move_constructible<V>::value) {
    return std::move(base_);
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      negation<conjunction<
          simple_view<V>,
          random_access_range<const V>,
          sized_range<const V>
      >>
  >::value, int> = 0>
  constexpr auto begin() {
    return begin_base::begin(base_, count_);
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      sized_range<const V>,
      random_access_range<const V>
  >::value, int> = 0>
  constexpr auto begin() const {
    return ranges::next(ranges::begin(base_), count_, ranges::end(base_));
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      negation<simple_view<V>>
  >::value, int> = 0>
  constexpr auto end() {
    return ranges::end(base_);
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      range<const V>
  >::value, int> = 0>
  constexpr auto end() const {
    return ranges::end(base_);
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      sized_range<V>
  >::value, int> = 0>
  constexpr auto size() {
    const auto s = ranges::size(base_);
    const auto c = static_cast<decltype(s)>(count_);
    return s < c ? 0 : s - c;
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      sized_range<const V>
  >::value, int> = 0>
  constexpr auto size() const {
    const auto s = ranges::size(base_);
    const auto c = static_cast<decltype(s)>(count_);
    return s < c ? 0 : s - c;
  }

 private:
  V base_{};
  range_difference_t<V> count_ = 0;
};

template<typename R>
constexpr drop_view<views::all_t<R>>
make_drop_view(R&& r, range_difference_t<R> count) {
  return drop_view<views::all_t<R>>(std::forward<R>(r), count);
}

#if __cplusplus >= 201703L

template<typename R>
drop_view(R&&, range_difference_t<R>) -> drop_view<views::all_t<R>>;

#endif

} // namespace ranges
} // namespace preview

template<typename T>
PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(preview::ranges::drop_view<T>)
    = preview::ranges::enable_borrowed_range<T>;

#endif // PREVIEW_RANGES_VIEWS_DROP_VIEW_H_
