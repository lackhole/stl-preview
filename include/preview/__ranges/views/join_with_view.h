//
// Created by yonggyulee on 2/13/24.
//

#ifndef PREVIEW_RANGES_VIEWS_JOIN_WITH_VIEW_H_
#define PREVIEW_RANGES_VIEWS_JOIN_WITH_VIEW_H_

#include <type_traits>
#include <utility>

#include "preview/__concepts/common_reference_with.h"
#include "preview/__concepts/common_with.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__concepts/derived_from.h"
#include "preview/__concepts/equality_comparable.h"
#include "preview/__iterator/detail/have_cxx20_iterator.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/iter_move.h"
#include "preview/__iterator/iter_swap.h"
#include "preview/__memory/addressof.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/bidirectional_range.h"
#include "preview/__ranges/common_range.h"
#include "preview/__ranges/simple_view.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/non_propagating_cache.h"
#include "preview/__ranges/range_difference_t.h"
#include "preview/__ranges/range_reference_t.h"
#include "preview/__ranges/range_rvalue_reference_t.h"
#include "preview/__ranges/range_value_t.h"
#include "preview/__ranges/sentinel_t.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/single.h"
#include "preview/__type_traits/common_reference.h"
#include "preview/__type_traits/common_type.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/maybe_const.h"
#include "preview/__type_traits/negation.h"
#include "preview/__utility/cxx20_rel_ops.h"
#include "preview/variant.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename RRef, typename Pattern>
using join_with_compatible = conjunction<
    common_with<range_value_t<RRef>, range_value_t<Pattern>>,
    common_reference_with<range_reference_t<RRef>, range_reference_t<Pattern>>,
    common_reference_with<range_rvalue_reference_t<RRef>, range_rvalue_reference_t<Pattern>>
  >;

template<typename V, typename Pattern, typename Derived, bool NoCache = forward_range<V>::value /* true */>
class join_with_view_base : public view_interface<Derived> {
  struct cache_wrapper {
    template <typename I>
    constexpr cache_wrapper(const I& i) noexcept(
        std::is_nothrow_constructible<non_propagating_cache<cache_wrapper>, decltype(*i)>::value)
        : val(*i) {}

    std::remove_cv_t<range_reference_t<V>> val;
  };
 protected:
  non_propagating_cache<cache_wrapper> inner_base_{};
};

template<typename V, typename Pattern, typename Derived>
class join_with_view_base<V, Pattern, Derived, false> : public join_with_view_base<V, Pattern, Derived, true> {
 protected:
  non_propagating_cache<iterator_t<V>> outer_it_{};
};


template<typename Base, typename InnerBase, typename PatternBase>
struct join_with_view_iterator_concept {
  using iterator_concept =
      conditional_t<
          conjunction<
              has_typename_type<range_reference<Base>>,
              std::is_reference<range_reference_t<Base>>,
              bidirectional_range<Base>,
              bidirectional_range<InnerBase>,
              bidirectional_range<PatternBase>,
              common_range<InnerBase>,
              common_range<PatternBase>
          >, bidirectional_iterator_tag,

          conjunction<
              has_typename_type<range_reference<Base>>,
              std::is_reference<range_reference_t<Base>>,
              forward_range<InnerBase>,
              forward_range<PatternBase>
          >, forward_iterator_tag,

          input_iterator_tag
      >;
};

template<typename Base, typename InnerBase, typename PatternBase,
         typename IterConcept = join_with_view_iterator_concept<Base, InnerBase, PatternBase>>
struct join_with_view_iterator_category
    : join_with_view_iterator_concept<Base, InnerBase, PatternBase>
{
  // iterator_category is defined iif IterConcept denotes forward_iterator_tag
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
  using iterator_category = iterator_ignore;
#endif
};

template<typename Base, typename InnerBase, typename PatternBase>
struct join_with_view_iterator_category<Base, InnerBase, PatternBase, forward_iterator_tag>
    : join_with_view_iterator_concept<Base, InnerBase, PatternBase>
{
 private:
  using OuterC = typename iterator_traits<iterator_t<Base>>::iterator_category;
  using InnerC = typename iterator_traits<iterator_t<InnerBase>>::iterator_category;
  using PatternC = typename iterator_traits<iterator_t<PatternBase>>::iterator_category;

 public:
  using iterator_category =
      conditional_t<
          negation<std::is_reference<
              common_reference_t<range_reference_t<InnerBase>, range_reference_t<PatternBase>>
          >>, input_iterator_tag,

          conjunction<
              derived_from<OuterC, bidirectional_iterator_tag>,
              derived_from<InnerC, bidirectional_iterator_tag>,
              derived_from<PatternC, bidirectional_iterator_tag>,
              common_range<range_reference_t<Base>>,
              common_range<PatternBase>
          >, bidirectional_iterator_tag,

          conjunction<
              derived_from<OuterC, forward_iterator_tag>,
              derived_from<InnerC, forward_iterator_tag>,
              derived_from<PatternC, forward_iterator_tag>
          >, forward_iterator_tag,

          input_iterator_tag
      >;
};

template<typename Base, typename InnerBase, typename PatternBase, bool Cache = forward_range<Base>::value/* false */>
class join_with_view_iterator_base : public join_with_view_iterator_category<Base, InnerBase, PatternBase> {};

template<typename Base, typename InnerBase, typename PatternBase>
class join_with_view_iterator_base<Base, InnerBase, PatternBase, true>
    : public join_with_view_iterator_category<Base, InnerBase, PatternBase>
{
 protected:
  join_with_view_iterator_base() = default;

  constexpr explicit join_with_view_iterator_base(iterator_t<Base>&& outer_it)
      : outer_it_(std::move(outer_it)) {}

  iterator_t<Base> outer_it_{};
};

} // namespace detail

// Part of implementations are taken from MSVC
template<typename V, typename Pattern>
class join_with_view : public detail::join_with_view_base<V, Pattern, join_with_view<V, Pattern>> {
 public:
  static_assert(input_range<V>::value, "Constraints not satisfied");
  static_assert(forward_range<Pattern>::value, "Constraints not satisfied");
  static_assert(view<V>::value, "Constraints not satisfied");
  static_assert(input_range<range_reference_t<V>>::value, "Constraints not satisfied");
  static_assert(view<Pattern>::value, "Constraints not satisfied");
  static_assert(detail::join_with_compatible<range_reference_t<V>, Pattern>::value, "Constraints not satisfied");

  template<bool Const> class iterator;
  template<bool Const> friend class iterator;
  template<bool Const> class sentinel;
  template<bool Const> friend class sentinel;

  template<bool Const>
  class iterator
      : public detail::join_with_view_iterator_base<
          maybe_const<Const, V>,
          range_reference_t<maybe_const<Const, V>>,
          maybe_const<Const, Pattern>>
    {
      using iterator_base = detail::join_with_view_iterator_base<
            maybe_const<Const, V>,
            range_reference_t<maybe_const<Const, V>>,
            maybe_const<Const, Pattern>>;

      friend class join_with_view;
      template<bool OtherConst> friend class sentinel;

      using Parent = maybe_const<Const, join_with_view>;
      using Base = maybe_const<Const, V>;
      using InnerBase = range_reference_t<Base>;
      using PatternBase = maybe_const<Const, Pattern>;

      using OuterIter = iterator_t<Base>;
      using InnerIter = iterator_t<InnerBase>;
      using PatternIter = iterator_t<PatternBase>;

     public:
      using value_type = common_type_t<range_value_t<InnerBase>, range_value_t<PatternBase>>;
      using difference_type = common_type_t<
            range_difference_t<Base>,
            range_difference_t<InnerBase>,
            range_difference_t<PatternBase>>;
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
      using pointer = void;
      using reference = common_reference_t<range_reference_t<InnerBase>, range_reference_t<PatternBase>>;
#endif

      iterator() = default;

      template<bool AntiConst, std::enable_if_t<conjunction<
          bool_constant<((Const != AntiConst) && Const)>,
          convertible_to<iterator_t<V>, iterator_t<Base>>,
          convertible_to<iterator_t<range_reference_t<V>>, iterator_t<InnerBase>>,
          convertible_to<iterator_t<Pattern>, iterator_t<Pattern>>
      >::value, int> = 0>
      constexpr iterator(iterator<AntiConst> i)
          : iterator_base(std::move(i.outer_it_)), parent_(i.parent_) {
        switch (i.inner_it_.index()) {
          case 0:
            inner_it_.template emplace<0>(std::move(
                preview::detail::variant_raw_get(i.inner_it_._base().union_, in_place_index<0>)));
            break;
          case 1:
            inner_it_.template emplace<1>(std::move(
                preview::detail::variant_raw_get(i.inner_it_._base().union_, in_place_index<1>)));
            break;
          default:
            throw bad_variant_access{};
        }
      }

      constexpr decltype(auto) operator*() const {
        using reference_type = common_reference_t<range_reference_t<InnerBase>, range_reference_t<PatternBase>>;
        return inner_it_.visit([](auto&& it) -> reference_type { return *it; });
      }

      constexpr iterator& operator++() {
        inner_it_.visit([](auto& it) { ++it; });
        satisfy();
        return *this;
      }

      template<typename IB = InnerBase, std::enable_if_t<conjunction<
          std::is_reference<IB>,
          forward_iterator<OuterIter>,
          forward_iterator<InnerIter>
      >::value == false, int> = 0>
      constexpr void operator++(int) {
        ++*this;
      }

      template<typename IB = InnerBase, std::enable_if_t<conjunction<
          std::is_reference<IB>,
          forward_iterator<OuterIter>,
          forward_iterator<InnerIter>
      >::value, int> = 0>
      constexpr iterator operator++(int) {
        auto tmp = *this;
        ++*this;
        return tmp;
      }

      template<typename IB = InnerBase, std::enable_if_t<conjunction<
          std::is_reference<IB>,
          bidirectional_range<Base>,
          bidirectional_range<InnerBase>,
          common_range<InnerBase>,
          bidirectional_range<PatternBase>,
          common_range<PatternBase>
      >::value, int> = 0>
      constexpr iterator& operator--() {
        using namespace preview::rel_ops;

        auto& outer_it = get_outer();
        if (outer_it == ranges::end(parent_->base_)) {
          --outer_it;
          inner_it_.template emplace<1>(ranges::end(get_inner()));
        }

        for (;;) {
          if (inner_it_.index() == 0) {
            auto& it = preview::detail::variant_raw_get(inner_it_._base().union_, in_place_index<0>);
            if (it == ranges::begin(parent_->pattern_)) {
              --outer_it;
              inner_it_.template emplace<1>(ranges::end(get_inner()));
            } else {
              break;
            }
          } else if (inner_it_.index() == 1) {
            auto& it = preview::detail::variant_raw_get(inner_it_._base().union_, in_place_index<1>);
            if (it == ranges::begin(get_inner())) {
              inner_it_.template emplace<0>(ranges::end(parent_->pattern_));
            } else {
              break;
            }
          } else {
            throw bad_variant_access{};
          }
        }

        inner_it_.visit([](auto& it) { --it; });
        return *this;
      }


      template<typename IB = InnerBase, std::enable_if_t<conjunction<
          std::is_reference<IB>,
          bidirectional_range<Base>,
          bidirectional_range<InnerBase>,
          common_range<InnerBase>,
          bidirectional_range<PatternBase>,
          common_range<PatternBase>
      >::value, int> = 0>
      constexpr iterator operator--(int) {
        auto tmp = *this;
        --*this;
        return tmp;
      }

      template<typename IB = InnerBase, std::enable_if_t<conjunction<
          std::is_reference<IB>,
          forward_range<Base>,
          equality_comparable<InnerIter>
      >::value, int> = 0>
      friend constexpr bool operator==(const iterator& x, const iterator& y) {
        if (x.outer_it_ != y.outer_it_) {
          return false;
        }

        return x.inner_it_ == y.inner_it_;
      }

      template<typename IB = InnerBase, std::enable_if_t<conjunction<
          std::is_reference<IB>,
          forward_range<Base>,
          equality_comparable<InnerIter>
      >::value, int> = 0>
      friend constexpr bool operator!=(const iterator& x, const iterator& y) {
        return !(x == y);
      }

      friend constexpr decltype(auto) iter_move(const iterator& i) {
        using R = common_reference_t<range_rvalue_reference_t<InnerBase>, range_rvalue_reference_t<PatternBase>>;
        return i.inner_it_.visit([](auto&& i) -> R {
          return ranges::iter_move(std::forward<decltype(i)>(i));
        });
      }

      template<typename IS = indirectly_swappable<iterator_t<InnerBase>, iterator_t<PatternBase>>, std::enable_if_t<IS::value, int> = 0>
      friend constexpr void iter_swap(const iterator& x, const iterator& y) {
        preview::visit(ranges::iter_swap, x.inner_it_, y.inner_it_);
      }

     private:
      template<typename B = Base, std::enable_if_t<forward_range<B>::value, int> = 0>
      constexpr iterator(Parent& parent, iterator_t<Base> outer_it)
          : iterator_base(std::move(outer_it)), parent_(preview::addressof(parent))
      {
        using namespace preview::rel_ops;
        if (this->outer_it_ != ranges::end(parent_->base_)) {
          inner_it_.template emplace<1>(ranges::begin(update_inner()));
          satisfy();
        }
      }

      template<typename B = Base, std::enable_if_t<forward_range<B>::value == false, int> = 0>
      constexpr iterator(Parent& parent)
          : parent_(preview::addressof(parent))
      {
        using namespace preview::rel_ops;
        if (*parent_->outer_it_ != ranges::end(parent_->base_)) {
          inner_it_.template emplace<1>(ranges::begin(update_inner()));
          satisfy();
        }
      }

      template<typename B = Base, std::enable_if_t<forward_range<B>::value, int> = 0>
      constexpr iterator_t<Base>& get_outer() noexcept {
          return this->outer_it_;
      }

      template<typename B = Base, std::enable_if_t<forward_range<B>::value == false, int> = 0>
      constexpr iterator_t<Base>& get_outer() noexcept {
          return *parent_->outer_it_;
      }

      template<typename B = Base, std::enable_if_t<forward_range<B>::value, int> = 0>
      constexpr const iterator_t<Base>& get_outer() const noexcept {
          return this->outer_it_;
      }

      template<typename B = Base, std::enable_if_t<forward_range<B>::value == false, int> = 0>
      constexpr const iterator_t<Base>& get_outer() const noexcept {
          return *parent_->outer_it_;
      }

      template<typename IB = InnerBase, std::enable_if_t<std::is_reference<IB>::value, int> = 0>
      constexpr decltype(auto) update_inner() {
        return *get_outer();
      }

      template<typename IB = InnerBase, std::enable_if_t<std::is_reference<IB>::value == false, int> = 0>
      constexpr decltype(auto) update_inner() {
        return parent_->inner_base_.emplace_deref(get_outer()).val;
      }

      template<typename IB = InnerBase, std::enable_if_t<std::is_reference<IB>::value, int> = 0>
      constexpr decltype(auto) get_inner() noexcept {
        return *get_outer();
      }

      template<typename IB = InnerBase, std::enable_if_t<std::is_reference<IB>::value == false, int> = 0>
      constexpr decltype(auto) get_inner() noexcept {
        return *parent_->inner_base_.val;
      }

      constexpr void satisfy() {
        using namespace preview::rel_ops;

        for (;;) {
          if (inner_it_.index() == 0) {
            if (preview::detail::variant_raw_get(inner_it_._base().union_, in_place_index<0>) != ranges::end(parent_->pattern_)) {
              break;
            }

            inner_it_.template emplace<1>(ranges::begin(update_inner()));
          } else {
            if (preview::detail::variant_raw_get(inner_it_._base().union_, in_place_index<1>) != ranges::end(get_inner())) {
              break;
            }

            auto& outer_it = get_outer();
            ++outer_it;
            if (outer_it == ranges::end(parent_->base_)) {
              emplace_first_or_not();
              break;
            }

            inner_it_.template emplace<0>(ranges::begin(parent_->pattern_));
          }
        }
      }

      template<typename IB = InnerBase, std::enable_if_t<std::is_reference<IB>::value, int> = 0>
      constexpr void emplace_first_or_not() {
        inner_it_.template emplace<0>();
      }

      template<typename IB = InnerBase, std::enable_if_t<std::is_reference<IB>::value == false, int> = 0>
      constexpr void emplace_first_or_not() { /* no op */ }

      Parent* parent_{};
      variant<iterator_t<PatternBase>, iterator_t<InnerBase>> inner_it_{};
    };

  template<bool Const>
  class sentinel {
    friend class join_with_view;

    using Parent = maybe_const<Const, join_with_view>;
    using Base = maybe_const<Const, V>;

   public:
    sentinel() = default;

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<((Const != AntiConst) && Const)>,
        convertible_to<sentinel_t<V>, sentinel_t<Base>>
    >::value, int> = 0>
    constexpr sentinel(sentinel<AntiConst> i)
        : end_(std::move(i.end_)) {}

    friend constexpr bool operator==(const iterator<Const>& x, const sentinel& y) {
      return y.equal(x);
    }

    friend constexpr bool operator!=(const iterator<Const>& x, const sentinel& y) {
      return !(x == y);
    }

    friend constexpr bool operator==(const sentinel& y, const iterator<Const>& x) {
      return y.equal(x);
    }

    friend constexpr bool operator!=(const sentinel& y, const iterator<Const>& x) {
      return !(x == y);
    }

   private:
    constexpr explicit sentinel(Parent& parent)
        : end_(ranges::end(parent.base_)) {}

    constexpr bool equal(const iterator<Const>& x) const {
      using namespace preview::rel_ops;
      return x.get_outer() == end_;
    }

    sentinel_t<Base> end_{};
  };

  join_with_view() = default;

  constexpr explicit join_with_view(V base, Pattern pattern)
      : base_(std::move(base)), pattern_(std::move(pattern)) {}

  template<typename R, std::enable_if_t<conjunction<
    constructible_from<V, views::all_t<R>>,
    constructible_from<Pattern, single_view<range_value_t<range_reference_t<V>>>>
  >::value, int> = 0>
  constexpr explicit join_with_view(R&& r, range_value_t<range_reference_t<V>> e)
      : base_(views::all(std::forward<R>(r))), pattern_(views::single(std::move(e))) {}


  template<typename V2 = V, std::enable_if_t<copy_constructible<V2>::value, int> = 0>
  constexpr V base() const& {
    return base_;
  }

  constexpr V base() && {
    return std::move(base_);
  }

  template<typename V2 = V, std::enable_if_t<forward_range<V2>::value, int> = 0>
  constexpr auto begin() {
    using Const = conjunction<
      simple_view<V2>,
      simple_view<Pattern>,
      std::is_reference<range_reference_t<V2>>
    >;
    return iterator<Const::value>{*this, ranges::begin(base_)};
  }

  template<typename V2 = V, std::enable_if_t<forward_range<V2>::value == false, int> = 0>
  constexpr iterator<false> begin() {
    this->outer_it_.emplace(ranges::begin(base_));
    return iterator<false>{*this};
  }

  template<typename V2 = V, std::enable_if_t<conjunction<
      forward_range<const V2>,
      forward_range<const Pattern>,
      std::is_reference<range_reference_t<const V2>>
  >::value, int> = 0>
  constexpr iterator<true> begin() const {
    return iterator<true>{*this, ranges::begin(base_)};
  }

  template<typename V2 = V, std::enable_if_t<conjunction<
    forward_range<V2>,
    std::is_reference<range_reference_t<V2>>,
    forward_range<range_reference_t<V2>>,
    common_range<V2>,
    common_range<range_reference_t<V2>>
  >::value, int> = 0>
  constexpr auto end() {
    using simple = conjunction<simple_view<V>, simple_view<Pattern>>;
    return iterator<simple::value>{*this, ranges::end(base_)};
  }

  template<typename V2 = V, std::enable_if_t<conjunction<
    forward_range<V2>,
    std::is_reference<range_reference_t<V2>>,
    forward_range<range_reference_t<V2>>,
    common_range<V2>,
    common_range<range_reference_t<V2>>
  >::value == false, int> = 0>
  constexpr auto end() {
    using simple = conjunction<simple_view<V>, simple_view<Pattern>>;
    return sentinel<simple::value>{*this};
  }

  template<typename V2 = V, std::enable_if_t<conjunction<
      forward_range<const V2>,
      forward_range<const Pattern>,
      std::is_reference<range_reference_t<const V2>>
  >::value, int> = 0>
  constexpr auto end() const {
    return end_impl(conjunction<
        forward_range<range_reference_t<const V>>,
        common_range<V>,
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

  V base_{};
  Pattern pattern_;
};

#if __cplusplus >= 201703L

template<typename R, typename P>
join_with_view(R&&, P&&) -> join_with_view<views::all_t<R>, views::all_t<P>>;

template<typename R>
join_with_view(R&&, range_value_t<range_reference_t<R>>)
    -> join_with_view<views::all_t<R>, single_view<range_value_t<range_reference_t<R>>>>;

#endif

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_JOIN_WITH_VIEW_H_
