//
// Created by yonggyulee on 1/31/24.
//

#ifndef PREVIEW_RANGES_VIEWS_JOIN_VIEW_H_
#define PREVIEW_RANGES_VIEWS_JOIN_VIEW_H_

#include <type_traits>
#include <utility>

#include "preview/__concepts/copyable.h"
#include "preview/__concepts/default_initializable.h"
#include "preview/__concepts/derived_from.h"
#include "preview/__concepts/equality_comparable.h"
#include "preview/__iterator/detail/have_cxx20_iterator.h"
#include "preview/__iterator/indirectly_swappable.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/iter_move.h"
#include "preview/__iterator/iter_swap.h"
#include "preview/__memory/addressof.h"
#include "preview/optional.h"
#include "preview/__ranges/bidirectional_range.h"
#include "preview/__ranges/common_range.h"
#include "preview/__ranges/simple_view.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/movable_box.h"
#include "preview/__ranges/non_propagating_cache.h"
#include "preview/__ranges/range_difference_t.h"
#include "preview/__ranges/range_reference_t.h"
#include "preview/__ranges/sentinel_t.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__ranges/views/all.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/common_type.h"
#include "preview/__type_traits/has_operator_arrow.h"
#include "preview/__type_traits/maybe_const.h"
#include "preview/__utility/as_const.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

template <class T>
constexpr T& as_lvalue(T&& x) noexcept {
  return static_cast<T&>(x);
}

template<typename Base>
using join_view_iterator_concept =
    conditional_t<
        conjunction<
            std::is_reference<range_reference_t<Base>>,
            bidirectional_range<Base>,
            bidirectional_range<range_reference_t<Base>>
        >, bidirectional_iterator_tag,

        conjunction<
            std::is_reference<range_reference_t<Base>>,
            forward_range<Base>,
            forward_range<range_reference_t<Base>>
        >, forward_iterator_tag,

        input_iterator_tag
    >;

// Defined only if IteratorConcept models forward_iterator_tag
template<typename Base, typename IteratorConcept = join_view_iterator_concept<Base>>
struct join_view_iterator_category {
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
  using iterator_category = iterator_ignore;
#endif
};

template<typename Base>
struct join_view_iterator_category<Base, forward_iterator_tag> {
 private:
  using OuterC = typename iterator_traits<iterator_t<Base>>::iterator_category;
  using InnerC = typename iterator_traits<iterator_t<range_reference_t<Base>>>::iterator_category;

 public:
  using iterator_category =
      conditional_t<
          conjunction<
              derived_from<OuterC, bidirectional_iterator_tag>,
              derived_from<InnerC, bidirectional_iterator_tag>
          >, bidirectional_iterator_tag,

          conjunction<
              derived_from<OuterC, forward_iterator_tag>,
              derived_from<InnerC, forward_iterator_tag>
          >, forward_iterator_tag,

          input_iterator_tag
      >;
};

template<typename Base, typename OuterIter, bool Present = forward_range<Base>::value /* true */>
struct join_view_iterator_outer_iter {
  join_view_iterator_outer_iter() = default;

  explicit join_view_iterator_outer_iter(OuterIter outer)
      : outer_(std::move(outer)) {}

  template<typename Parent>
  constexpr OuterIter& outer(Parent&) {
    return outer_;
  }

  template<typename Parent>
  constexpr const OuterIter& outer(Parent&) const {
    return outer_;
  }

  OuterIter outer_ = OuterIter();
};

template<typename Base, typename OuterIter>
struct join_view_iterator_outer_iter<Base, OuterIter, false> {
  join_view_iterator_outer_iter() = default;

  explicit join_view_iterator_outer_iter(OuterIter) {}

  template<typename Parent>
  constexpr OuterIter& outer(Parent& p) {
    return *p->outer_;
  }

  template<typename Parent>
  constexpr const OuterIter& outer(Parent& p) const {
    return *p->outer_;
  }
};

template<typename JoinView, typename V, bool UseCache = negation<forward_range<V>>::value /* true */>
class join_view_outer_base : public view_interface<JoinView> {
 public:



 protected:
  non_propagating_cache<iterator_t<V>> outer_;
};

template<typename JoinView, typename V>
class join_view_outer_base<JoinView, V, false> : public view_interface<JoinView> {
 public:

};

template<typename JoinView, typename V, bool UseCache = negation<std::is_reference<range_reference_t<V>>>::value /* true */>
class join_view_inner_base : public join_view_outer_base<JoinView, V> {
 public:

 protected:
  non_propagating_cache<std::remove_cv_t<range_reference_t<V>>> inner_;
};

template<typename JoinView, typename V>
class join_view_inner_base<JoinView, V, false> : public join_view_outer_base<JoinView, V> {
 public:
};

} // namespace detail

template<typename V>
class join_view : public detail::join_view_inner_base<join_view<V>, V> {
 private:
  template <bool Const>
  using InnerRng = range_reference_t<maybe_const<Const, V>>;

 public:
  static_assert(input_range<V>::value, "Constraints not satisfied");
  static_assert(view<V>::value, "Constraints not satisfied");
  static_assert(input_range<range_reference_t<V>>::value, "Constraints not satisfied");

  template<bool Const> class iterator;
  template<bool Const> friend class iterator;
  template<bool Const> class sentinel;

  template<bool Const>
  class iterator
      : public detail::join_view_iterator_category<maybe_const<Const, V>>
      , private detail::join_view_iterator_outer_iter<
            maybe_const<Const, V>, // Base
            iterator_t<maybe_const<Const, V>> // OuterIter
        >
  {
    using Parent = maybe_const<Const, join_view>;
    using Base = maybe_const<Const, V>;
    using OuterIter = iterator_t<Base>;
    using InnerIter = iterator_t<range_reference_t<Base>>;
    static constexpr bool ref_is_glvalue = std::is_reference<range_reference_t<Base>>::value;

    friend class join_view;
    template<bool> friend class sentinel;

    using outer_iter_base = detail::join_view_iterator_outer_iter<
        maybe_const<Const, V>,
        iterator_t<maybe_const<Const, V>>
    >;

    constexpr OuterIter& outer() {
      return outer_iter_base::outer(parent_);
    }

    constexpr const OuterIter& outer() const {
      return outer_iter_base::outer(parent_);
    }

    constexpr decltype(auto) update_inner_impl(const iterator_t<Base>& x, std::true_type /* ref_is_glvalue */) {
      return *x;
    }
    constexpr decltype(auto) update_inner_impl(const iterator_t<Base>& x, std::false_type /* ref_is_glvalue */) {
      return parent_->inner_.emplace_deref(x);
    }

    constexpr decltype(auto) update_inner(const iterator_t<Base>& x) {
      return update_inner_impl(x, bool_constant<ref_is_glvalue>{});
    }

    constexpr void satisfy_impl(std::true_type /* ref_is_glvalue */) {
      for (; outer() != ranges::end(parent_->base_); ++outer()) {
        auto&& inner = update_inner(outer());
        inner_ = ranges::begin(inner);
        if (*inner_ != ranges::end(inner))
          return;
      }
      inner_.reset();
    }
    constexpr void satisfy_impl(std::false_type /* ref_is_glvalue */) {
      for (; outer() != ranges::end(parent_->base_); ++outer()) {
        auto&& inner = update_inner(outer());;
        inner_ = ranges::begin(inner);
        if (inner_ != ranges::end(inner))
          return;
      }
    }

    constexpr void satisfy() {
      satisfy_impl(bool_constant<ref_is_glvalue>{});
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        forward_range<Base>
    >::value, int> = 0>
    constexpr iterator(Parent& parent, OuterIter outer)
        : outer_iter_base(std::move(outer))
        , parent_(preview::addressof(parent))
    {
      satisfy();
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        negation<forward_range<Base>>
    >::value, int> = 0>
    constexpr explicit iterator(Parent& parent)
        : parent_(preview::addressof(parent))
    {
      satisfy();
    }

   public:
    using iterator_concept = detail::join_view_iterator_concept<Base>;

    using value_type = range_value_t<range_reference_t<Base>>;
    using difference_type = common_type_t<range_difference_t<Base>, range_difference_t<range_reference_t<Base>>>;
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
    using pointer = void;
    using reference = decltype(**std::declval<movable_box<InnerIter>&>());
#endif

    iterator() = default;

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<((Const != AntiConst) && Const)>,
        convertible_to<iterator_t<V>, OuterIter>,
        convertible_to<iterator_t<InnerRng<false>>, InnerIter>
    >::value, int> = 0>
    constexpr iterator(iterator<AntiConst> i)
        : outer_iter_base(std::move(i.outer()))
        , inner_(std::move(i.inner_))
        , parent_(i.parent_) {}

    constexpr decltype(auto) operator*() const {
      return **inner_;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        has_operator_arrow<InnerIter>,
        copyable<InnerIter>
    >::value, int> = 0>
    constexpr decltype(auto) operator->() const {
      return *inner_;
    }

    constexpr iterator& operator++() {
      return pre_increment(bool_constant<ref_is_glvalue>{});
    }

   private:
    constexpr iterator& pre_increment(std::true_type) {
      if (++*inner_ == ranges::end(detail::as_lvalue(*outer()))) {
        ++outer();
        satisfy();
      }
      return *this;
    }
    constexpr iterator& pre_increment(std::false_type) {
      if (++*inner_ == ranges::end(detail::as_lvalue(*parent_->inner_))) {
        ++outer();
        satisfy();
      }
      return *this;
    }

   public:
    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        negation<conjunction<
            bool_constant<ref_is_glvalue>,
            forward_range<Base>,
            forward_range<range_reference_t<Base>>
        >>
    >::value, int> = 0>
    constexpr void operator++(int) {
      ++*this;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        bool_constant<ref_is_glvalue>,
        forward_range<Base>,
        forward_range<range_reference_t<Base>>
    >::value, int> = 0>
    constexpr iterator operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        bool_constant<ref_is_glvalue>,
        bidirectional_range<Base>,
        bidirectional_range<range_reference_t<Base>>,
        common_range<range_reference_t<Base>>
    >::value, int> = 0>
    constexpr iterator& operator--() {
      if (outer() == ranges::end(parent_->base_))
        inner_ = ranges::end(detail::as_lvalue(*--outer()));
      while (*inner_ == ranges::begin(detail::as_lvalue(*outer())))
        inner_ = ranges::end(detail::as_lvalue(*--outer()));
      --*inner_;
      return *this;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        bool_constant<ref_is_glvalue>,
        bidirectional_range<Base>,
        bidirectional_range<range_reference_t<Base>>,
        common_range<range_reference_t<Base>>
    >::value, int> = 0>
    constexpr iterator operator--(int) {
      auto tmp = *this;
      --*this;
      return tmp;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        bool_constant<ref_is_glvalue>,
        forward_range<Base>,
        equality_comparable<iterator_t<range_reference_t<Base>>>
    >::value, int> = 0>
    friend constexpr bool operator==(const iterator& x, const iterator& y) {
      return (x.outer_ == y.outer_) && (x.inner_ == y.inner_);
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        bool_constant<ref_is_glvalue>,
        forward_range<Base>,
        equality_comparable<iterator_t<range_reference_t<Base>>>
    >::value, int> = 0>
    friend constexpr bool operator!=(const iterator& x, const iterator& y) {
      return !(x == y);
    }

    friend constexpr decltype(auto) iter_move(const iterator& i)
        noexcept(noexcept(ranges::iter_move(*i.inner_)))
    {
      return ranges::iter_move(*i.inner_);
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        indirectly_swappable<InnerIter>
    >::value, int> = 0>
    friend constexpr void iter_swap(const iterator& x, const iterator& y)
        noexcept(noexcept(ranges::iter_swap(*x.inner_, *y.inner_)))
    {
      ranges::iter_swap(*x.inner_, *y.inner_);
    }

   private:

    optional<InnerIter> inner_;
    Parent* parent_ = nullptr;
  };

  template<bool Const>
  class sentinel {
    using Parent = maybe_const<Const, join_view>;
    using Base = maybe_const<Const, V>;

   public:
    sentinel() = default;

    constexpr explicit sentinel(Parent& parent)
        : end_(ranges::end(parent.base_)) {}

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<((Const != AntiConst) && Const)>,
        convertible_to<sentinel_t<V>, sentinel_t<Base>>
    >::value, int> = 0>
    constexpr sentinel(sentinel<AntiConst> s)
        : end_(std::move(s.end_)) {}

    template<bool OtherConst, std::enable_if_t<
        sentinel_for<sentinel_t<Base>, iterator_t<maybe_const<OtherConst, V>>>
    ::value, int> = 0>
    friend constexpr bool operator==(const iterator<OtherConst>& x, const sentinel& y) {
      return x.outer() == y.end_;
    }

    template<bool OtherConst, std::enable_if_t<
        sentinel_for<sentinel_t<Base>, iterator_t<maybe_const<OtherConst, V>>>
    ::value, int> = 0>
    friend constexpr bool operator!=(const iterator<OtherConst>& x, const sentinel& y) {
      return !(x == y);
    }

    template<bool OtherConst, std::enable_if_t<
        sentinel_for<sentinel_t<Base>, iterator_t<maybe_const<OtherConst, V>>>
    ::value, int> = 0>
    friend constexpr bool operator==(const sentinel& y, const iterator<OtherConst>& x) {
      return x == y;
    }

    template<bool OtherConst, std::enable_if_t<
        sentinel_for<sentinel_t<Base>, iterator_t<maybe_const<OtherConst, V>>>
    ::value, int> = 0>
    friend constexpr bool operator!=(const sentinel& y, const iterator<OtherConst>& x) {
      return !(x == y);
    }

   private:
    sentinel_t<Base> end_{};
  };

  template<bool B = default_initializable<V>::value, std::enable_if_t<B, int> = 0>
  constexpr join_view() {}

  constexpr explicit join_view(V base)
      : base_(std::move(base)) {}

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      copy_constructible<V>
  >::value, int> = 0>
  constexpr V base() const& {
    return base_;
  }

  constexpr V base() && {
    return std::move(base_);
  }

  // vvvvvvvvvv begin vvvvvvvvvv
  constexpr auto begin() {
    return begin_impl(forward_range<V>{});
  }

 private:
  constexpr auto begin_impl(std::true_type /* forward_range<V> */) {
    using B = conjunction<simple_view<V>, std::is_reference<InnerRng<false>>>;
    return iterator<B::value>{*this, ranges::begin(base_)};
  }
  constexpr auto begin_impl(std::false_type /* forward_range<V> */) {
    this->outer_ = ranges::begin(base_);
    return iterator<false>{*this};
  }
  // ^^^^^^^^^^ begin ^^^^^^^^^^

 public:
  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      forward_range<const V>,
      std::is_reference<range_reference_t<const V>>,
      input_range<range_reference_t<const V>>
  >::value, int> = 0>
  constexpr auto begin() const {
    return iterator<true>{*this, ranges::begin(base_)};
  }

  // vvvvvvvvvv end() vvvvvvvvvv

  constexpr auto end() {
    return end_impl(conjunction<
        forward_range<V>,
        std::is_reference<InnerRng<false>>, forward_range<InnerRng<false>>,
        common_range<V>, common_range<InnerRng<false>>
    >{});
  }

 private:
  constexpr auto end_impl(std::true_type) {
    return iterator<simple_view<V>::value>{*this, ranges::end(base_)};
  }
  constexpr auto end_impl(std::false_type) {
    return sentinel<simple_view<V>::value>{*this};
  }
  // ^^^^^^^^^^ end() ^^^^^^^^^^

  // vvvvvvvvvv end() const vvvvvvvvvv
 public:
  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      forward_range<const V>,
      std::is_reference<range_reference_t<const V>>,
      input_range<range_reference_t<const V>>
  >::value, int> = 0>
  constexpr auto end() const {
    return end_impl(conjunction<
        forward_range<range_reference_t<const V>>,
        common_range<const V>,
        common_range<range_reference_t<const V>>
    >{});
  }

 private:
  constexpr auto end_impl(std::true_type) const {
    return iterator<true>{*this, ranges::end(base_)};
  }
  constexpr auto end_impl(std::false_type) const {
    return sentinel<true>{*this};
  }
  // ^^^^^^^^^^ end() const ^^^^^^^^^^

  V base_{};
};

#if __cplusplus >= 201703L

template<typename R>
explicit join_view(R&&) -> join_view<views::all_t<R>>;

#endif

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_JOIN_VIEW_H_
