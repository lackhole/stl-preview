//
// Created by yonggyulee on 2024/01/02.
//

#ifndef PREVIEW_RANGES_VIEWS_TAKE_VIEW_H_
#define PREVIEW_RANGES_VIEWS_TAKE_VIEW_H_

#include <algorithm>
#include <type_traits>
#include <utility>

#include "preview/__concepts/convertible_to.h"
#include "preview/__concepts/copy_constructible.h"
#include "preview/__concepts/default_initializable.h"
#include "preview/__iterator/counted_iterator.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/simple_view.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/random_access_range.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/range_difference_t.h"
#include "preview/__ranges/sentinel_t.h"
#include "preview/__ranges/size.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/views/all.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/maybe_const.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__utility/cxx20_rel_ops.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
#endif

namespace preview {
namespace ranges {

template<typename V>
class take_view : public view_interface<take_view<V>> {
 public:
  static_assert(view<V>::value, "Constraints not satisfied");

  template<bool Const>
  class sentinel {
    using Base = maybe_const<Const, V>;
   public:
    sentinel() = default;

    constexpr explicit sentinel(sentinel_t<Base> end)
        : end_(std::move(end)) {}

    template<bool NoConst, std::enable_if_t<conjunction<
        bool_constant<Const>,
        bool_constant<Const != NoConst>,
        convertible_to<sentinel_t<V>, sentinel_t<Base>>
    >::value, int> = 0>
    constexpr sentinel(sentinel<NoConst> s)
        : end_(std::move(s.end_)) {}

    constexpr sentinel_t<Base> base() const {
      return end_;
    }

    friend constexpr bool
    operator==(const counted_iterator<iterator_t<Base>>& y, const sentinel& x) {
      using namespace preview::rel_ops;
      return y.count() == 0 || y.base() == x.end_;
    }

    friend constexpr bool
    operator==(const sentinel& x, const counted_iterator<iterator_t<Base>>& y) {
      return y == x;
    }

    friend constexpr bool
    operator!=(const counted_iterator<iterator_t<Base>>& y, const sentinel& x) {
      return !(y == x);
    }

    friend constexpr bool
    operator!=(const sentinel& x, const counted_iterator<iterator_t<Base>>& y) {
      return !(y == x);
    }

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<Const != AntiConst>,
        sentinel_for< sentinel_t<Base>,
                      iterator_t<maybe_const<AntiConst, V>> >
    >::value, int> = 0>
    friend constexpr bool
    operator==(const counted_iterator<iterator_t<maybe_const<AntiConst, V>>>& y, const sentinel& x) {
      using namespace preview::rel_ops;
      return y.count() == 0 || y.base() == x.end_;
    }

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<Const != AntiConst>,
        sentinel_for< sentinel_t<Base>,
                      iterator_t<maybe_const<AntiConst, V>> >
    >::value, int> = 0>
    friend constexpr bool
    operator==(const sentinel& x, const counted_iterator<iterator_t<maybe_const<AntiConst, V>>>& y) {
      return y == x;
    }

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<Const != AntiConst>,
        sentinel_for< sentinel_t<Base>,
                      iterator_t<maybe_const<AntiConst, V>> >
    >::value, int> = 0>
    friend constexpr bool
    operator!=(const counted_iterator<iterator_t<maybe_const<AntiConst, V>>>& y, const sentinel& x) {
      return !(y == x);
    }

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<Const != AntiConst>,
        sentinel_for< sentinel_t<Base>,
                      iterator_t<maybe_const<AntiConst, V>> >
    >::value, int> = 0>
    friend constexpr bool
    operator!=(const sentinel& x, const counted_iterator<iterator_t<maybe_const<AntiConst, V>>>& y) {
      return !(y == x);
    }

   private:
    sentinel_t<Base> end_;
  };

  template<bool B = default_initializable<V>::value, std::enable_if_t<B, int> = 0>
  constexpr take_view() {}

  constexpr explicit take_view(V base, range_difference_t<V> count)
      : base_(std::move(base))
      , count_(count) {}

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      copy_constructible<V>
  >::value, int> = 0>
  constexpr V base() const& {
    return base_;
  }

  constexpr V base() && {
    return std::move(base_);
  }


  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      negation< simple_view<V> >,
      sized_range<V>,
      random_access_range<V>
  >::value, int> = 0>
  constexpr auto begin() {
    return ranges::begin(base_);
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      negation< simple_view<V> >,
      sized_range<V>,
      negation< random_access_range<V> >
  >::value, int> = 0>
  constexpr auto begin() {
    using I = remove_cvref_t<iterator_t<V>>;
    return counted_iterator<I>(ranges::begin(base_), ranges::range_difference_t<V>(this->size()));
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      negation< simple_view<V> >,
      negation< sized_range<V> >
  >::value, int> = 0>
  constexpr auto begin() {
    using I = remove_cvref_t<iterator_t<V>>;
    return counted_iterator<I>(ranges::begin(base_), count_);
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      range<const V>,
      sized_range<const V>,
      random_access_range<const V>
  >::value, int> = 0>
  constexpr auto begin() const {
    return ranges::begin(base_);
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      range<const V>,
      sized_range<const V>,
      negation< random_access_range<const V> >
  >::value, int> = 0>
  constexpr auto begin() const {
    using I = remove_cvref_t<iterator_t<const V>>;
    return counted_iterator<I>(ranges::begin(base_), ranges::range_difference_t<const V>(this->size()));
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      range<const V>,
      negation< sized_range<const V> >
  >::value, int> = 0>
  constexpr auto begin() const {
    using I = remove_cvref_t<iterator_t<const V>>;
    return counted_iterator<I>(ranges::begin(base_), count_);
  }


  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      negation< simple_view<V> >,
      sized_range<V>,
      random_access_range<V>
  >::value, int> = 0>
  constexpr auto end() {
    return ranges::begin(base_) + ranges::range_difference_t<V>(this->size());
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      negation< simple_view<V> >,
      sized_range<V>,
      negation< random_access_range<V> >
  >::value, int> = 0>
  constexpr default_sentinel_t end() {
    return default_sentinel;
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      negation< simple_view<V> >,
      negation< sized_range<V> >
  >::value, int> = 0>
  constexpr sentinel<false> end() {
    return sentinel<false>(ranges::end(base_));
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      range<const V>,
      sized_range<const V>,
      random_access_range<const V>
  >::value, int> = 0>
  constexpr auto end() const {
    return ranges::begin(base_) + ranges::range_difference_t<const V>(this->size());
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      range<V>,
      sized_range<V>,
      negation< random_access_range<V> >
  >::value, int> = 0>
  constexpr default_sentinel_t end() const {
    return default_sentinel;
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      range<V>,
      negation< sized_range<V> >
  >::value, int> = 0>
  constexpr sentinel<true> end() const {
    return sentinel<true>(ranges::end(base_));
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      ranges::sized_range<V>
  >::value, int> = 0>
  constexpr auto size() {
    auto n = ranges::size(base_);
    return (std::min)(n, static_cast<decltype(n)>(count_));
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      ranges::sized_range<const V>
  >::value, int> = 0>
  constexpr auto size() const {
    auto n = ranges::size(base_);
    return (std::min)(n, static_cast<decltype(n)>(count_));
  }

 private:
  V base_{};
  range_difference_t<V> count_ = 0;
};

template<typename R>
constexpr take_view<views::all_t<R>> make_take_view(R&& r, range_difference_t<R> count) {
  return take_view<views::all_t<R>>(std::forward<R>(r), count);
}

#if __cplusplus >= 201703L

template<typename R>
take_view(R&&, ranges::range_difference_t<R>) -> take_view<views::all_t<R>>;

#endif

} // namespace ranges
} // namespace preview

template<typename T>
PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(preview::ranges::take_view<T>)
    = preview::ranges::enable_borrowed_range<T>;

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // PREVIEW_RANGES_VIEWS_TAKE_VIEW_H_
