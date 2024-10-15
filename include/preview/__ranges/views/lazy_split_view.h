//
// Created by YongGyu Lee on 2024. 7. 1..
//

#ifndef PREVIEW_RANGES_VIEWS_LAZY_SPLIT_VIEW_H_
#define PREVIEW_RANGES_VIEWS_LAZY_SPLIT_VIEW_H_

#include <memory>
#include <type_traits>
#include <utility>

#include "preview/core.h"
#include "preview/__algorithm/ranges/find.h"
#include "preview/__algorithm/ranges/mismatch.h"
#include "preview/__concepts/constructible_from.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__concepts/copy_constructible.h"
#include "preview/__concepts/default_initializable.h"
#include "preview/__concepts/derived_from.h"
#include "preview/__functional/equal_to.h"
#include "preview/__iterator/default_sentinel_t.h"
#include "preview/__iterator/indirectly_comparable.h"
#include "preview/__iterator/indirectly_swappable.h"
#include "preview/__iterator/iter_swap.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/detail/have_cxx20_iterator.h"
#include "preview/__memory/addressof.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/non_propagating_cache.h"
#include "preview/__ranges/range_difference_t.h"
#include "preview/__ranges/range_value_t.h"
#include "preview/__ranges/simple_view.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/single.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/maybe_const.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

struct require_constant_size_tester {
  template<typename R>
  static constexpr auto test(int)
      -> decltype(std::integral_constant<decltype(R::size()), R::size()>{}, std::true_type{});
  template<typename R>
  static constexpr auto test(...) -> std::false_type;
};

template<typename R>
struct require_constant_size
    : decltype(require_constant_size_tester::test<std::remove_reference_t<R>>(0)) {};

template<typename R, bool = conjunction<
    sized_range<R>,
    require_constant_size<std::remove_reference_t<R>>
>::value /* false */>
struct tiny_range : std::false_type {};

template<typename R>
struct tiny_range<R, true> : bool_constant<(std::remove_reference_t<R>::size() <= 1)> {};

} // namespace detail

template<typename V, typename Pattern>
class lazy_split_view
    : public view_interface<lazy_split_view<V, Pattern>>
{
  template<bool Const> friend struct inner_iterator;
  template<bool Const> friend struct outer_iterator;

  template<bool Const>
  struct inner_iterator;

  template<typename Base, bool = forward_range<Base>::value>
  struct outer_iterator_category {
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
    using iterator_category = iterator_ignore;
#endif
  };
  template<typename Base>
  struct outer_iterator_category<Base, true> {
    iterator_t<Base> current_{};
    using iterator_category = input_iterator_tag;
  };

  template<bool Const, bool VIsForwardRange>
  struct outer_iterator_current {
    outer_iterator_current() = default;
    template<typename Unused>
    outer_iterator_current(Unused&&) {}
  };
  template<bool Const>
  struct outer_iterator_current<Const, true> {
   private:
    using Base = maybe_const<Const, V>;

   public:
    outer_iterator_current() = default;
    explicit outer_iterator_current(iterator_t<Base> current)
        : current_(std::move(current)) {}

    template<bool AntiConst, std::enable_if_t<(AntiConst != Const) && Const, int> = 0>
    outer_iterator_current(outer_iterator_current<AntiConst, true>& i)
        : current_(std::move(i.current_)) {}

    iterator_t<Base> current_{};
  };

  template<bool Const>
  struct outer_iterator
      : outer_iterator_category<maybe_const<Const, V>>
      , outer_iterator_current<Const, forward_range<V>::value>
  {
   private:
    using current_base = outer_iterator_current<Const, forward_range<V>::value>;

    using Parent = maybe_const<Const, lazy_split_view>;
    using Base = maybe_const<Const, V>;
    Parent* parent_ = nullptr;
    bool trailing_empty_ = false;

    friend struct outer_iterator<true>;
    template<bool Const2> friend struct inner_iterator;

    template<typename Last, typename Begin>
    void increment_if_tiny(std::false_type, Last&&, Begin&&) noexcept {}
    template<typename Last, typename Begin>
    void increment_if_tiny(std::true_type, Last&& last, Begin&& begin) {
      current() = ranges::find(std::move(current()), last, *begin);
      if (current() != last) {
        ++current();
        if (current() == last)
          trailing_empty_ = true;
      }
    }

    constexpr outer_iterator post_increment(std::true_type) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    constexpr void post_increment(std::false_type) {
      ++*this;
    }

          iterator_t<Base>& current_impl(std::true_type)        { return current_base::current_; }
    const iterator_t<Base>& current_impl(std::true_type) const  { return current_base::current_; }
          iterator_t<V>&    current_impl(std::false_type)       { return *parent_->current_;     }
    const iterator_t<V>&    current_impl(std::false_type) const { return *parent_->current_;     }
    decltype(auto) current()       { return current_impl(forward_range<Base>{}); }
    decltype(auto) current() const { return current_impl(forward_range<Base>{}); }

   public:
    using iterator_concept =
        std::conditional_t<forward_range<Base>::value, forward_iterator_tag, input_iterator_tag>;
    struct value_type : view_interface<value_type> {
     private:
      friend struct outer_iterator;

      outer_iterator i_{};

      constexpr explicit value_type(outer_iterator i)
          : i_(std::move(i)) {}

     public:
      inner_iterator<Const> begin() const;

      constexpr default_sentinel_t end() const noexcept {
        return default_sentinel;
      }
    };
    using difference_type = range_difference_t<Base>;
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
    using pointer = void;
    using reference = value_type;
#endif

    outer_iterator() = default;

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        negation<forward_range<Base>>
    >::value, int> = 0>
    PREVIEW_CONSTEXPR_AFTER_CXX17 explicit outer_iterator(Parent& parent)
        : parent_(preview::addressof(parent)) {}

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        forward_range<Base>
    >::value, int> = 0>
    PREVIEW_CONSTEXPR_AFTER_CXX17 outer_iterator(Parent& parent, iterator_t<Base> current)
        : current_base(std::move(current))
        , parent_(preview::addressof(parent)) {}

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<(AntiConst != Const) && Const>,
        convertible_to<iterator_t<V>, iterator_t<Base>>
    >::value, int> = 0>
    constexpr outer_iterator(outer_iterator<AntiConst> i)
        : current_base(i)
        , parent_(i.parent_)
        , trailing_empty_(i.trailing_empty_) {}

    constexpr value_type operator*() const {
      return value_type{*this};
    }

    constexpr outer_iterator& operator++() {
      const auto end = ranges::end(parent_->base_);
      if (current() == end) {
        trailing_empty_ = false;
        return *this;
      }

      auto p = make_subrange(parent_->pattern_);
      if (p.begin() == p.end()) {
        ++current();
      } else if PREVIEW_CONSTEXPR_AFTER_CXX17 (detail::tiny_range<Pattern>::value) {
        increment_if_tiny(detail::tiny_range<Pattern>{}, end, p.begin());
      } else {
        do {
          auto b_p = ranges::mismatch(current(), end, p.begin(), p.end());
          if (b_p.in2 == p.end()) {
            current() = b_p.in1;
            if (current() == end)
              trailing_empty_ = true;
            break;
          }
        } while (++current() != end);
      }

      return *this;
    }

    constexpr decltype(auto) operator++(int) {
      return post_increment(forward_range<Base>{});
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        forward_range<Base>
    >::value, int> = 0>
    friend constexpr bool operator==(const outer_iterator& x, const outer_iterator& y) {
      return x.current() == y.current() && x.trailing_empty_ == y.trailing_empty_;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        forward_range<Base>
    >::value, int> = 0>
    friend constexpr bool operator!=(const outer_iterator& x, const outer_iterator& y) {
      return !(x == y);
    }

    friend constexpr bool operator==(const outer_iterator& x, default_sentinel_t) {
      using namespace rel_ops;
      return x.current() == ranges::end(x.parent_->base_) && !x.trailing_empty_;
    }

    friend constexpr bool operator==(default_sentinel_t, const outer_iterator& x) {
      return x == default_sentinel;
    }

    friend constexpr bool operator!=(const outer_iterator& x, default_sentinel_t) {
      return !(x == default_sentinel);
    }

    friend constexpr bool operator!=(default_sentinel_t, const outer_iterator& x) {
      return !(x == default_sentinel);
    }
  };

  template<typename Base, bool = forward_range<Base>::value /* false */>
  struct inner_iterator_category {
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
    using iterator_category = iterator_ignore;
#endif
  };

  template<typename Base>
  struct inner_iterator_category<Base, true> {
   private:
    using base_iterator_category = typename iterator_traits<iterator_t<Base>>::iterator_category;
   public:
    using iterator_category = std::conditional_t<
        derived_from<base_iterator_category, forward_iterator_tag>::value, forward_iterator_tag,
        base_iterator_category
    >;
  };

  template<bool Const>
  struct inner_iterator : inner_iterator_category<maybe_const<Const, V>> {
   private:
    using Base = maybe_const<Const, V>;
    outer_iterator<Const> i_{};
    bool incremented_ = false;

    constexpr bool equal_with(const inner_iterator& other) const {
      return i_.current() == other.i_.current();
    }

   public:
    using iterator_concept = typename outer_iterator<Const>::iterator_concept;
    using value_type = range_value_t<Base>;
    using difference_type = range_difference_t<Base>;
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
    using pointer = void;
    using reference = decltype(*std::declval<iterator_t<Base>>());
#endif

    inner_iterator() = default;

    constexpr explicit inner_iterator(outer_iterator<Const> i)
        : i_(std::move(i)) {}

    constexpr const iterator_t<Base>& base() const & noexcept {
      return i_.current();
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        forward_range<V>
    >::value, int> = 0>
    constexpr iterator_t<Base> base() && {
      return std::move(i_.current());
    }

    constexpr decltype(auto) operator*() const {
      return *i_.current();
    }

    constexpr inner_iterator& operator++() {
      return pre_increment(forward_range<Base>{});
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        forward_range<Base>
    >::value, int> = 0>
    constexpr inner_iterator operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        negation<forward_range<Base>>
    >::value, int> = 0>
    constexpr void operator++(int) {
      ++*this;
    }

    friend constexpr bool operator==(const inner_iterator& x, const inner_iterator& y) {
      return x.equal_with(y);
    }
    friend constexpr bool operator!=(const inner_iterator& x, const inner_iterator& y) {
      return !(x == y);
    }

    friend constexpr bool operator==(const inner_iterator& x, default_sentinel_t) {
      return x.compare_with_default_sentinel(detail::tiny_range<Pattern>{});
    }
    friend constexpr bool operator!=(const inner_iterator& x, default_sentinel_t) {
      return !(x == default_sentinel);
    }
    friend constexpr bool operator==(default_sentinel_t, const inner_iterator& x) {
      return x == default_sentinel;
    }
    friend constexpr bool operator!=(default_sentinel_t, const inner_iterator& x) {
      return !(x == default_sentinel);
    }


    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        indirectly_swappable<iterator_t<Base>>
    >::value, int> = 0>
    friend constexpr void iter_swap(const inner_iterator& x, const inner_iterator& y)
        noexcept(noexcept(x.iter_swap_impl(y)))
    {
      x.iter_swap_impl(y);
    }

    friend constexpr decltype(auto) iter_move(const inner_iterator& i) noexcept(noexcept(i.iter_move_impl())) {
      return i.iter_move_impl();
    }

   private:
    constexpr inner_iterator& pre_increment(std::true_type /* forward_range<Base> */) {
      ++i_.current();
      return *this;
    }
    constexpr inner_iterator& pre_increment(std::false_type /* forward_range<Base> */) {
      return pre_increment_empty_pattern(bool_constant<(Pattern::size() == 0)>{});
    }
    constexpr inner_iterator& pre_increment_empty_pattern(std::true_type) {
      return *this;
    }
    constexpr inner_iterator& pre_increment_empty_pattern(std::false_type) {
      return pre_increment(std::true_type{});
    }

    constexpr decltype(auto) post_increment(std::true_type /* forward_range<Base> */) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }
    constexpr void post_increment(std::false_type /* forward_range<Base> */) {
      ++*this;
    }

    constexpr bool compare_with_default_sentinel(std::true_type /* tiny_range<Pattern> */) const {
      using namespace preview::rel_ops;

      auto p = preview::ranges::make_subrange(i_.parent_->pattern_);
      auto end = ranges::end(i_.parent_->base_);

      const auto& cur = i_.current();
      if (cur == end) return true;
      if (p.begin() == p.end()) return incremented_;
      return *cur == *p.begin();
    }
    constexpr bool compare_with_default_sentinel(std::false_type /* tiny_range<Pattern> */) const {
      using namespace preview::rel_ops;

      auto p = preview::ranges::make_subrange(i_.parent_->pattern_);
      auto pcur = p.begin();
      auto pend = p.end();
      auto end = ranges::end(i_.parent_->base_);

      auto cur = i_.current();
      if (cur == end) return true;
      if (pcur == pend) return incremented_;
      do {
        if (!(*cur == *pcur)) return false;
        if (++pcur == pend) return true;
      } while (!(++cur == end));
      return false;
    }

    constexpr void iter_swap_impl(const inner_iterator& y) const
        noexcept(noexcept(ranges::iter_swap(i_.current(), y.i_.current())))
    {
      ranges::iter_swap(i_.current(), y.i_.current());
    }

    constexpr decltype(auto) iter_move_impl() const noexcept(noexcept(ranges::iter_move(i_.current()))) {
      return ranges::iter_move(i_.current());
    }
  };

 public:
  static_assert(input_range<V>::value, "Constraints not satisfied");
  static_assert(forward_range<Pattern>::value, "Constraints not satisfied");
  static_assert(view<V>::value, "Constraints not satisfied");
  static_assert(view<Pattern>::value, "Constraints not satisfied");
  static_assert(indirectly_comparable<iterator_t<V>, iterator_t<Pattern>, ranges::equal_to>::value, "Constraints not satisfied");
  static_assert(disjunction<forward_range<V>, detail::tiny_range<Pattern>>::value, "Constraints not satisfied");

  template<bool B = default_initializable<V>::value && default_initializable<Pattern>::value, std::enable_if_t<B, int> = 0>
  constexpr lazy_split_view() {}

  constexpr explicit lazy_split_view(V base, Pattern pattern)
      : base_(std::move(base))
      , pattern_(std::move(pattern)) {}

  template<typename R, std::enable_if_t<conjunction<
      input_range<R>,
      constructible_from<V, views::all_t<R>>,
      constructible_from<Pattern, single_view<range_value_t<R>>>
  >::value, int> = 0>
  constexpr explicit lazy_split_view(R&& r, range_value_t<R> e)
      : base_(views::all(std::forward<R>(r)))
      , pattern_(views::single(std::move(e))) {}

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      copy_constructible<V>
  >::value, int> = 0>
  constexpr V base() const & {
    return base_;
  }

  constexpr V base() && {
    return std::move(base_);
  }

  constexpr auto begin() {
    return begin_impl(forward_range<V>{});
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      forward_range<V>,
      forward_range<const V>
  >::value, int> = 0>
  constexpr auto begin() const {
    return outer_iterator<true>{*this, ranges::begin(base_)};
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      forward_range<V>,
      common_range<V>
  >::value, int> = 0>
  constexpr auto end() {
    return outer_iterator<conjunction<simple_view<V>, simple_view<Pattern>>::value>{
        *this, ranges::end(base_)
    };
  }

  constexpr auto end() const {
    return end_impl(conjunction<forward_range<V>, forward_range<const V>, common_range<const V>>{});
  }

 private:
  constexpr auto begin_impl(std::true_type /* forward_range<V> */) {
    return outer_iterator<conjunction<simple_view<V>, simple_view<Pattern>>::value>{
        *this, ranges::begin(base_)
    };
  }
  constexpr auto begin_impl(std::false_type /* forward_range<V> */) {
    current_ = ranges::begin(base_);
    return outer_iterator<false>{*this};
  }

  constexpr auto end_impl(std::true_type) const {
    return outer_iterator<true>{*this, ranges::end(base_)};
  }
  constexpr auto end_impl(std::false_type) const {
    return default_sentinel;
  }

  V base_{};
  Pattern pattern_{};
  non_propagating_cache<iterator_t<V>> current_;
};

template<typename V, typename Pattern>
template<bool Const>
typename lazy_split_view<V, Pattern>::template inner_iterator<Const>
lazy_split_view<V, Pattern>::outer_iterator<Const>::value_type::begin() const {
  return lazy_split_view::inner_iterator<Const>{i_};
}

#if PREVIEW_CXX_VERSION >= 17

template<class R, class P>
lazy_split_view(R&&, P&&) -> lazy_split_view<views::all_t<R>, views::all_t<P>>;

template<typename R>
lazy_split_view(R&&, range_value_t<R>)
    -> lazy_split_view<
            std::enable_if_t<input_range<R>::value,
        views::all_t<R>>,
        single_view<range_value_t<R>>
    >;

#endif

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_LAZY_SPLIT_VIEW_H_
