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
#include "preview/__iterator/indirectly_swappable.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/iter_move.h"
#include "preview/__iterator/iter_swap.h"
#include "preview/__memory/addressof.h"
#include "preview/__ranges/bidirectional_range.h"
#include "preview/__ranges/common_range.h"
#include "preview/__ranges/simple_view.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/movable_box.h"
#include "preview/__ranges/range_difference_t.h"
#include "preview/__ranges/range_reference_t.h"
#include "preview/__ranges/sentinel_t.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__ranges/views/all.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/common_type.h"
#include "preview/__type_traits/has_operator_arrow.h"
#include "preview/__type_traits/maybe_const.h"
#include "preview/__utility/as_const.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename Base>
using join_view_iterator_concept =
    std::conditional_t<
        conjunction<
            std::is_reference<range_reference_t<Base>>,
            bidirectional_range<Base>,
            bidirectional_range<range_reference_t<Base>>
        >::value,
        bidirectional_iterator_tag,
    std::conditional_t<
        conjunction<
            std::is_reference<range_reference_t<Base>>,
            forward_range<Base>,
            forward_range<range_reference_t<Base>>
        >::value,
        forward_iterator_tag,
        input_iterator_tag
    >>;

// Defined only if IteratorConcept models forward_iterator_tag
template<typename Base, typename IteratorConcept = join_view_iterator_concept<Base>>
struct join_view_iterator_category {
#if __cplusplus < 202002L
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
      std::conditional_t<
          conjunction<
              derived_from<OuterC, bidirectional_iterator_tag>,
              derived_from<InnerC, bidirectional_iterator_tag>
          >::value,
          bidirectional_iterator_tag,
      std::conditional_t<
          conjunction<
              derived_from<OuterC, forward_iterator_tag>,
              derived_from<InnerC, forward_iterator_tag>
          >::value,
          forward_iterator_tag,
          input_iterator_tag
      >>;
};

} // namespace detail

template<typename V>
class join_view : public view_interface<join_view<V>> {
 public:
  static_assert(input_range<V>::value, "Constraints not satisfied");
  static_assert(view<V>::value, "Constraints not satisfied");
  static_assert(input_range<range_reference_t<V>>::value, "Constraints not satisfied");

 private:
  template <bool Const>
  using InnerRng = range_reference_t<maybe_const<Const, V>>;

 public:
  template<bool Const> class iterator;
  template<bool Const> friend class iterator;
  template<bool Const> class sentinel;

  template<bool Const>
  class iterator : detail::join_view_iterator_category<maybe_const<Const, V>> {
    using Parent = maybe_const<Const, join_view>;
    using Base = maybe_const<Const, V>;
    using OuterIter = iterator_t<Base>;
    using InnerIter = iterator_t<range_reference_t<Base>>;
    static constexpr bool ref_is_glvalue = std::is_reference<range_reference_t<Base>>::value;

    friend class join_view;
    template<bool> friend class sentinel;

   public:
    using iterator_concept = detail::join_view_iterator_concept<Base>;

    using value_type = range_value_t<range_reference_t<Base>>;
    using difference_type = common_type_t<range_difference_t<Base>, range_difference_t<range_reference_t<Base>>>;
#if __cplusplus < 202002L
    using pointer = void;
    using reference = decltype(**std::declval<movable_box<InnerIter>&>());
#endif

    template<typename O = OuterIter, std::enable_if_t<conjunction<
        default_initializable<O>,
        default_initializable<InnerIter>
    >::value, int> = 0>
    iterator()
        : outer_(), inner_(), parent_(nullptr) {}

    constexpr iterator(Parent& parent, OuterIter outer)
        : outer_(std::move(outer)), parent_(preview::addressof(parent))
    {
      satisfy();
    }

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<((Const != AntiConst) && Const)>,
        convertible_to<iterator_t<V>, OuterIter>,
        convertible_to<iterator_t<InnerRng<false>>, InnerIter>
    >::value, int> = 0>
    constexpr iterator(iterator<AntiConst> i)
        : outer_(std::move(i.outer_)), inner_(std::move(i.inner_)), parent_(i.parent_) {}

    constexpr decltype(auto) operator*() const {
      return **inner_;
    }

    template<typename II = InnerIter, std::enable_if_t<conjunction<
        has_operator_arrow<II>,
        copyable<II>
    >::value, int> = 0>
    constexpr decltype(auto) operator->() const {
      return *inner_;
    }

    template<bool B = ref_is_glvalue, std::enable_if_t<B, int> = 0>
    constexpr iterator& operator++() {
      auto&& inner_rng = *outer_;
      if (++*inner_ == ranges::end(inner_rng)) {
        ++outer_;
        satisfy();
      }
      return *this;
    }

    template<bool B = ref_is_glvalue, std::enable_if_t<B == false, int> = 0>
    constexpr iterator& operator++() {
      auto&& inner_rng = *parent_->inner_;
      if (++*inner_ == ranges::end(inner_rng)) {
        ++outer_;
        satisfy();
      }
      return *this;
    }

    template<bool B = ref_is_glvalue, std::enable_if_t<conjunction<
        bool_constant<B>,
        forward_range<Base>,
        forward_range<range_reference_t<Base>>
    >::value == false, int> = 0>
    constexpr void operator++(int) {
      ++*this;
    }

    template<bool B = ref_is_glvalue, std::enable_if_t<conjunction<
        bool_constant<B>,
        forward_range<Base>,
        forward_range<range_reference_t<Base>>
    >::value, int> = 0>
    constexpr iterator operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    template<bool B = ref_is_glvalue, std::enable_if_t<conjunction<
        bool_constant<B>,
        bidirectional_range<Base>,
        bidirectional_range<range_reference_t<Base>>,
        common_range<range_reference_t<Base>>
    >::value, int> = 0>
    constexpr iterator& operator--() {
      if (outer_ == ranges::end(parent_->base_))
        inner_ = ranges::end(*--outer_);
      while (*inner_ == ranges::begin(*outer_))
        inner_ = ranges::end(*--outer_);
      --*inner_;
      return *this;
    }

    template<bool B = ref_is_glvalue, std::enable_if_t<conjunction<
        bool_constant<B>,
        bidirectional_range<Base>,
        bidirectional_range<range_reference_t<Base>>,
        common_range<range_reference_t<Base>>
    >::value, int> = 0>
    constexpr iterator operator--(int) {
      auto tmp = *this;
      --*this;
      return tmp;
    }

    constexpr void satisfy() {
      satisfy_impl(bool_constant<ref_is_glvalue>{});
    }

    template<bool B = ref_is_glvalue, std::enable_if_t<conjunction<
        bool_constant<B>,
        equality_comparable<iterator_t<Base>>,
        equality_comparable<iterator_t<range_reference_t<Base>>>
    >::value, int> = 0>
    friend constexpr bool operator==(const iterator& x, const iterator& y) {
      return (x.outer_ == y.outer_) && (*x.inner_ == *y.inner_);
    }

    template<bool B = ref_is_glvalue, std::enable_if_t<conjunction<
        bool_constant<B>,
        equality_comparable<iterator_t<Base>>,
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

    template<typename II = InnerIter, std::enable_if_t<indirectly_swappable<II>::value, int> = 0>
    friend constexpr void iter_swap(const iterator& x, const iterator& y)
        noexcept(noexcept(ranges::iter_swap(*x.inner_, *y.inner_)))
    {
      ranges::iter_swap(x.inner_, y.inner_);
    }

   private:
    constexpr OuterIter& get_outer() noexcept {
      return get_outer(forward_range<Base>{});
    }
    constexpr const OuterIter& get_outer() const noexcept {
      return get_outer(forward_range<Base>{});
    }
    constexpr OuterIter& get_outer_impl(std::true_type /* forward_range */) noexcept {
        return outer_;
    }
    constexpr OuterIter& get_outer_impl(std::false_type /* forward_range */) noexcept {
        return *parent_->outer_;
    }
    constexpr OuterIter& get_outer_impl(std::true_type /* forward_range */) const noexcept {
        return outer_;
    }
    constexpr OuterIter& get_outer_impl(std::false_type /* forward_range */) const noexcept {
        return *parent_->outer_;
    }

    constexpr decltype(auto) update_inner_impl(const iterator_t<Base>& x, std::true_type /* ref_is_glvalue */) {
      return *x;
    }
    constexpr decltype(auto) update_inner_impl(const iterator_t<Base>& x, std::false_type /* ref_is_glvalue */) {
      return parent_->inner_.emplace(x);
    }

    constexpr decltype(auto) update_inner(const iterator_t<Base>& x) {
      return update_inner_impl(x, bool_constant<ref_is_glvalue>{});
    }

    constexpr void satisfy_impl(std::true_type /* ref_is_glvalue */) {
      for (; outer_ != ranges::end(parent_->base_); ++outer_) {
        auto&& inner = update_inner(outer_);
        inner_ = ranges::begin(inner);
        if (*inner_ != ranges::end(inner))
          return;
      }

      inner_ = InnerIter();
    }
    constexpr void satisfy_impl(std::false_type /* ref_is_glvalue */) {
      for (; outer_ != ranges::end(parent_->base_); ++outer_) {
        auto&& inner = update_inner(outer_);;
        inner_ = ranges::begin(inner);
        if (inner_ != ranges::end(inner))
          return;
      }
    }

    OuterIter outer_;
    movable_box<InnerIter> inner_;
    Parent* parent_;
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

    friend constexpr bool operator==(const iterator<Const>& x, const sentinel& y) {
      using namespace preview::rel_ops;
      return x.get_outer() == y.end_;
    }

    friend constexpr bool operator==(const sentinel& y, const iterator<Const>& x) {
      return x == y;
    }

    friend constexpr bool operator!=(const iterator<Const>& x, const sentinel& y) {
      return !(x == y);
    }

    friend constexpr bool operator!=(const sentinel& y, const iterator<Const>& x) {
      return !(x == y);
    }

   private:
    sentinel_t<Base> end_;
  };

  template<typename V2 = V, std::enable_if_t<default_initializable<V2>::value, int> = 0>
  join_view() : base_(V()) {}

  constexpr explicit join_view(V base) : base_(std::move(base)) {}

  template<typename V2 = V, std::enable_if_t<copy_constructible<V2>::value, int> = 0>
  constexpr V base() const& {
    return base_;
  }

  constexpr V base() && {
    return std::move(base_);
  }

  constexpr auto begin() {
    using B = conjunction<simple_view<V>, std::is_reference<range_reference_t<V>>>;
    return iterator<B::value>{*this, ranges::begin(base_)};
  }

  template<typename V2 = V, std::enable_if_t<conjunction<
      input_range<const V2>,
      std::is_reference<range_reference_t<const V2>>
  >::value, int> = 0>
  constexpr auto begin() const {
    return iterator<true>{*this, ranges::begin(base_)};
  }

  constexpr auto end() {
    return end_impl(conjunction<
        forward_range<V>,
        std::is_reference<range_reference_t<V>>,
        forward_range<range_reference_t<V>>,
        common_range<V>,
        common_range<range_reference_t<V>>
      >{});
  }

  template<typename V2 = V, std::enable_if_t<conjunction<
      input_range<const V2>,
      std::is_reference<range_reference_t<const V2>>
  >::value, int> = 0>
  constexpr auto end() const {
    return end_impl(conjunction<
        forward_range<const V>,
        std::is_reference<range_reference_t<const V>>,
        forward_range<range_reference_t<const V>>,
        common_range<const V>,
        common_range<range_reference_t<const V>>
      >{});
  }

 private:
  constexpr auto end_impl(std::true_type) {
    return iterator<simple_view<V>::value>{*this, ranges::end(base_)};
  }
  constexpr auto end_impl(std::false_type) {
    return sentinel<simple_view<V>::value>{*this};
  }

  constexpr auto end_impl(std::true_type) const {
    return iterator<true>{*this, ranges::end(base_)};
  }
  constexpr auto end_impl(std::false_type) const {
    return sentinel<true>{*this};
  }

  V base_;
};

#if __cplusplus >= 201703L

template<typename R>
explicit join_view(R&&) -> join_view<views::all_t<R>>;

#endif

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_JOIN_VIEW_H_
