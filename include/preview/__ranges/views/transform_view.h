//
// Created by yonggyulee on 1/11/24.
//

#ifndef PREVIEW_RANGES_VIEWS_TRANSFORM_VIEW_H_
#define PREVIEW_RANGES_VIEWS_TRANSFORM_VIEW_H_

#include <memory>
#include <type_traits>
#include <utility>

#include "preview/__concepts/convertible_to.h"
#include "preview/__concepts/copy_constructible.h"
#include "preview/__concepts/equality_comparable.h"
#include "preview/__concepts/invocable.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__functional/invoke.h"
#include "preview/__memory/addressof.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/bidirectional_range.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/movable_box.h"
#include "preview/__ranges/random_access_range.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/range_difference_t.h"
#include "preview/__ranges/range_reference_t.h"
#include "preview/__ranges/sentinel_t.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/maybe_const.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename C>
struct transform_view_iterator_category_2 {
  using type = C;
};
template<>
struct transform_view_iterator_category_2<contiguous_iterator_tag> {
  using type = random_access_iterator_tag;
};

} // namespace detail

template<typename V, typename F>
class transform_view : public view_interface<transform_view<V, F>> {
 public:
  static_assert(input_range<V>::value, "Constraints not satisfied");
  static_assert(copy_constructible<F>::value, "Constraints not satisfied");
  static_assert(view<V>::value, "Constraints not satisfied");
  static_assert(std::is_object<F>::value, "Constraints not satisfied");
  static_assert(regular_invocable<F&, range_reference_t<V>>::value, "Constraints not satisfied");

 private:

  template<typename Base, bool v = forward_range<Base>::value /* false */>
  struct transform_view_iterator_category {
#if __cplusplus < 202002L
    using iterator_category = iterator_ignore;
#endif
  };
  template<typename Base>
  struct transform_view_iterator_category<Base, true> {
    using iterator_category =
      std::conditional_t<
        std::is_reference<invoke_result_t<F&, range_reference_t<Base>>>::value,
          detail::transform_view_iterator_category_2<typename iterator_traits<iterator_t<Base>>::iterator_category>,
          input_iterator_tag
      >;
  };

 public:
  template<bool Const> class iterator;
  template<bool Const> friend class iterator;

  template<bool Const> class sentinel;

  template<bool Const>
  class iterator : public transform_view_iterator_category<std::conditional_t<Const, const V, V>> {
    using Parent = maybe_const<Const, transform_view>;
    using Base = maybe_const<Const, V>;

    template<bool B> friend class sentinel;
    friend class transform_view;
   public:
    using iterator_concept =
      std::conditional_t<
        random_access_range<Base>::value, random_access_iterator_tag,
      std::conditional_t<
        bidirectional_range<Base>::value, bidirectional_iterator_tag,
      std::conditional_t<
        forward_range<Base>::value, forward_iterator_tag,
        input_iterator_tag
      >>>;
    using value_type = remove_cvref_t<invoke_result_t<F&, range_reference_t<Base>>>;
    using difference_type = range_difference_t<Base>;
#if __cplusplus < 202002L
    using pointer = void;
    using reference = invoke_result_t<decltype(std::declval<F&>()), decltype(*std::declval<iterator_t<Base>&>())>;
#endif

    iterator() = default;

    constexpr iterator(Parent& parent, iterator_t<Base> current)
        : current_(std::move(current)), parent_(preview::addressof(parent)) {}

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<((Const != AntiConst) && Const)>,
        convertible_to<iterator_t<V>, iterator_t<Base>>
    >::value, int> = 0>
    constexpr iterator(iterator<AntiConst> i)
        : current_(std::move(i.current_)), parent_(i.praent_) {}

    constexpr const iterator_t<Base>& base() const & noexcept { return current_; }
    constexpr iterator_t<Base> base() && { return std::move(current_); }

    constexpr decltype(auto) operator*() const {
      return preview::invoke(*parent_->func_, *current_);
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    constexpr decltype(auto) operator[](difference_type n) const {
      return preview::invoke(*parent_->func_, current_[n]);
    }

    constexpr iterator& operator++() {
      ++current_;
      return *this;
    }

    template<typename B = Base, std::enable_if_t<forward_range<B>::value == false, int> = 0>
    constexpr void operator++(int) {
      ++current_;
    }

    template<typename B = Base, std::enable_if_t<forward_range<B>::value, int> = 0>
    constexpr iterator operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    template<typename B = Base, std::enable_if_t<bidirectional_range<B>::value, int> = 0>
    constexpr iterator& operator--() {
      --current_;
      return *this;
    }

    template<typename B = Base, std::enable_if_t<bidirectional_range<B>::value, int> = 0>
    constexpr iterator operator--(int) {
      auto tmp = *this;
      --*this;
      return tmp;
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    constexpr iterator& operator+=(difference_type n) {
      current_ += n;
      return *this;
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    constexpr iterator& operator-=(difference_type n) {
      current_ -= n;
      return *this;
    }

    template<typename B = Base, std::enable_if_t<equality_comparable<iterator_t<B>>::value, int> = 0>
    friend constexpr bool operator==(const iterator& x, const iterator& y) {
      return x.current_ == y.current_;
    }

    template<typename B = Base, std::enable_if_t<equality_comparable<iterator_t<B>>::value, int> = 0>
    friend constexpr bool operator!=(const iterator& x, const iterator& y) {
      return !(x == y);
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    friend constexpr bool operator<(const iterator& x, const iterator& y) {
      return x.current_ < y.current_;
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    friend constexpr bool operator>(const iterator& x, const iterator& y) {
      return y < x;
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    friend constexpr bool operator<=(const iterator& x, const iterator& y) {
      return !(y < x);
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    friend constexpr bool operator>=(const iterator& x, const iterator& y) {
      return !(x < y);
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    friend constexpr iterator operator+(iterator i, difference_type n) {
      return iterator{*i.parent_, i.current_ + n};
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    friend constexpr iterator operator+(difference_type n, iterator i) {
      return iterator{*i.parent_, i.current_ + n};
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    friend constexpr iterator operator-(iterator i, difference_type n) {
      return iterator{*i.parent_, i.current_ - n};
    }

    template<typename B = Base, std::enable_if_t<sized_sentinel_for<iterator_t<B>, iterator_t<B>>::value, int> = 0>
    friend constexpr difference_type operator-(const iterator& x, const iterator& y) {
      return x.current_ - y.current_;
    }

    friend constexpr decltype(auto) iter_move(const iterator& i)
        noexcept(noexcept(preview::invoke(std::declval<const F&>(), *i.current_)))
    {
      return iter_move_ref(*i);
    }

   private:
    template<typename T, std::enable_if_t<std::is_rvalue_reference<T&&>::value, int> = 0>
    static constexpr decltype(auto) iter_move_ref(T&& ref) {
      return std::forward<T>(ref);
    }
    template<typename T>
    static constexpr decltype(auto) iter_move_ref(T& ref) {
      return std::move(ref);
    }

    iterator_t<Base> current_;
    Parent* parent_ = nullptr;
  };

  template<bool Const>
  class sentinel {
    using Parent = maybe_const<Const, transform_view>;
    using Base = maybe_const<Const, V>;

   public:
    sentinel() = default;

    constexpr explicit sentinel(sentinel_t<Base> end)
        : end_(std::move(end)) {}

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<((Const != AntiConst) && Const)>,
        convertible_to<sentinel_t<V>, sentinel_t<Base>>
    >::value, int> = 0>
    constexpr sentinel(sentinel<AntiConst> i)
        : end_(std::move(i.end_)) {}

    constexpr sentinel_t<Base> base() const {
      return end_;
    }

    friend constexpr bool operator==(const transform_view::iterator<Const>& x, const sentinel& y) {
      using namespace preview::rel_ops;
      return x.current_ == y.end_;
    }

    friend constexpr bool operator==(const sentinel& y, const transform_view::iterator<Const>& x) {
      return x == y;
    }

    friend constexpr bool operator!=(const transform_view::iterator<Const>& x, const sentinel& y) {
      return !(x == y);
    }

    friend constexpr bool operator!=(const sentinel& y, const transform_view::iterator<Const>& x) {
      return !(x == y);
    }

    template<typename B = Base, std::enable_if_t<sized_sentinel_for<sentinel_t<B>, iterator_t<B>>::value, int> = 0>
    friend constexpr range_difference_t<B>
    operator-(const iterator<Const>& x, const sentinel& y) {
      return x.current_ - y.end_;
    }

    template<typename B = Base, std::enable_if_t<sized_sentinel_for<sentinel_t<B>, iterator_t<B>>::value, int> = 0>
    friend constexpr range_difference_t<B>
    operator-(const sentinel& y, const iterator<Const>& x) {
      return y.end_ - x.current_;
    }

   private:
    sentinel_t<Base> end_;
  };

  transform_view() = default;

  constexpr explicit transform_view(V base, F func) : base_(std::move(base)), func_(func) {}

  template<typename V2 = V, std::enable_if_t<copy_constructible<V2>::value, int> = 0>
  constexpr V base() const& {
   return base_;
  }

  constexpr V base() && {
    return std::move(base_);
  }

  constexpr iterator<false> begin() {
    return iterator<false>{*this, ranges::begin(base_)};
  }

  template<typename V2 = const V, typename F2 = const F, std::enable_if_t<conjunction<
      range<V2>,
      regular_invocable<F2&, ranges::range_reference_t<V2>>
  >::value, int> = 0>
  constexpr iterator<true> begin() const {
    return iterator<true>{*this, ranges::begin(base_)};
  }

  template<typename V2 = V, std::enable_if_t<common_range<V2>::value == false, int> = 0>
  constexpr sentinel<false> end() {
    return sentinel<false>{ranges::end(base_)};
  }

  template<typename V2 = V, std::enable_if_t<common_range<V2>::value, int> = 0>
  constexpr iterator<false> end() {
    return iterator<false>{*this, ranges::end(base_)};
  }

  template<typename V2 = const V, typename F2 = const F, std::enable_if_t<conjunction<
      range<V2>,
      negation<common_range<V2>>,
      regular_invocable<F2&, range_reference_t<V2>>
  >::value, int> = 0>
  constexpr sentinel<true> end() const {
    return sentinel<true>{ranges::end(base_)};
  }

  template<typename V2 = const V, typename F2 = const F, std::enable_if_t<conjunction<
      common_range<V2>,
      regular_invocable<F2&, range_reference_t<V2>>
  >::value, int> = 0>
  constexpr iterator<true> end() const {
    return iterator<true>{*this, ranges::end(base_)};
  }

  template<typename V2 = V, std::enable_if_t<sized_range<V2>::value, int> = 0>
  constexpr auto size() {
    return ranges::size(base_);
  }

  template<typename V2 = const V, std::enable_if_t<sized_range<V2>::value, int> = 0>
  constexpr auto size() const {
    return ranges::size(base_);
  }

 private:
  V base_;
  movable_box<F> func_;
};

template<typename R, typename F, std::enable_if_t<conjunction<
    input_range<views::all_t<R>>,
    copy_constructible<F>,
    std::is_object<F>,
    regular_invocable<F&, range_reference_t<views::all_t<R>>>
>::value, int> = 0>
constexpr transform_view<views::all_t<R>, F>
make_transform_view(R&& r, F f) {
  return transform_view<views::all_t<R>, F>(std::forward<R>(r), std::move(f));
}

#if __cplusplus >= 201703L

template<typename R, typename F>
transform_view(R&&, F) -> transform_view<views::all_t<R>, F>;

#endif

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_TRANSFORM_VIEW_H_
