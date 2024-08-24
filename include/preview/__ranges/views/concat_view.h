//
// Created by YongGyu Lee on 2/13/24.
//

#ifndef CONCAT_VIEW_H_
#define CONCAT_VIEW_H_

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__concepts/dereferenceable.h"
#include "preview/__concepts/derived_from.h"
#include "preview/__concepts/swappable_with.h"
#include "preview/__iterator/default_sentinel_t.h"
#include "preview/__iterator/indirectly_swappable.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/iter_move.h"
#include "preview/__iterator/iter_reference_t.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__iterator/next.h"
#include "preview/__memory/addressof.h"
#include "preview/__ranges/bidirectional_range.h"
#include "preview/__ranges/common_range.h"
#include "preview/__ranges/simple_view.h"
#include "preview/__ranges/distance.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/random_access_range.h"
#include "preview/__ranges/range_difference_t.h"
#include "preview/__ranges/range_value_t.h"
#include "preview/__ranges/range_rvalue_reference_t.h"
#include "preview/__ranges/size.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/views/all.h"
#include "preview/__tuple/apply.h"
#include "preview/__tuple/tuple_fold.h"
#include "preview/__tuple/tuple_transform.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/common_type.h"
#include "preview/__type_traits/common_reference.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_referenceable.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/make_unsigned_like.h"
#include "preview/__type_traits/maybe_const.h"
#include "preview/__utility/integer_sequence.h"
#include "preview/__utility/type_sequence.h"
#include "preview/variant.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename... Rs>
struct concat_reference : common_reference<range_reference_t<Rs>...> {};
template<typename... Rs>
struct concat_value : common_type<range_value_t<Rs>...> {};
template<typename... Rs>
struct concat_rvalue_reference : common_reference<range_rvalue_reference_t<Rs>...> {};

template<typename Ref, typename RRef, typename It, bool = /* false */ dereferenceable<It>::value, typename = void>
struct concat_indirectly_readable_impl : std::false_type {};

template<typename Ref, typename RRef, typename It>
struct concat_indirectly_readable_impl<Ref, RRef, It, true, void_t<decltype(ranges::iter_move(std::declval<const It&>()))>>
    : conjunction<
        convertible_to<decltype(*std::declval<const It&>()), Ref>,
        convertible_to<decltype(ranges::iter_move(std::declval<const It&>())), RRef>
    > {};

template<typename... Rs>
struct concat_indirectly_readable
    : conjunction<
        common_reference_with<typename concat_reference<Rs...>::type&&,
                              typename concat_value<Rs...>::type&>,
        common_reference_with<typename concat_reference<Rs...>::type&&,
                              typename concat_rvalue_reference<Rs...>::type&&>,
        common_reference_with<typename concat_rvalue_reference<Rs...>::type&&,
                              typename concat_value<Rs...>::type const &>,
        concat_indirectly_readable_impl<typename concat_reference<Rs...>::type,
                                        typename concat_rvalue_reference<Rs...>::type,
                                        iterator_t<Rs>>...
    > {};

template<bool /* false */, typename... Rs> struct concatable_impl2 : std::false_type {};
template<bool /* false */, typename... Rs> struct concatable_impl : std::false_type {};

template<typename... Rs>
struct concatable_impl2<true, Rs...>
    : concat_indirectly_readable<Rs...> {};

template<typename... Rs>
struct concatable_impl<true, Rs...>
    : concatable_impl2<
        conjunction<
            has_typename_type< concat_reference<Rs...>        >,
            has_typename_type< concat_rvalue_reference<Rs...> >,
            has_typename_type< concat_value<Rs...>            >
        >::value,
        Rs...
    > {};

template<typename... Rs>
struct concatable
    : concatable_impl<
        conjunction<
            has_typename_type< range_reference<Rs> >...,
            has_typename_type< range_value<Rs> >...,
            has_typename_type< range_rvalue_reference<Rs> >...
        >::value,
        Rs...
    > {};

template<bool Const, typename... Rs>
struct concat_is_random_access;

template<bool Const, typename R>
struct concat_is_random_access<Const, R>
    : random_access_range<maybe_const<Const, R>> {};

template<bool Const, typename R, typename... Rs>
struct concat_is_random_access<Const, R, Rs...>
    : conjunction<
        random_access_range<maybe_const<Const, R>>,
        common_range<maybe_const<Const, R>>,
        concat_is_random_access<Const, Rs...>
    > {};

template<bool Const, typename... Rs>
struct concat_is_bidirectional;

template<bool Const, typename R>
struct concat_is_bidirectional<Const, R>
    : bidirectional_range<maybe_const<Const, R>> {};

template<bool Const, typename R, typename... Rs>
struct concat_is_bidirectional<Const, R, Rs...>
    : conjunction<
        bidirectional_range<maybe_const<Const, R>>,
        common_range<maybe_const<Const, R>>,
        concat_is_random_access<Const, Rs...>
    > {};

template<bool Const, typename... Rs>
struct concat_is_sized;

template<bool Const, typename R>
struct concat_is_sized<Const, R>
    : sized_sentinel_for<sentinel_t<maybe_const<Const, R>>, iterator_t<maybe_const<Const, R>>> {};

template<bool Const, typename R, typename... Rs>
struct concat_is_sized<Const, R, Rs...>
    : conjunction<
        sized_sentinel_for<sentinel_t<maybe_const<Const, R>>, iterator_t<maybe_const<Const, R>>>,
        sized_sentinel_for<sentinel_t<maybe_const<Const, Rs>>, iterator_t<maybe_const<Const, Rs>>>...,
        sized_range<maybe_const<Const, Rs>>...
    > {};

} // namespace detail

// Modified from ranges-v3 library
template<typename... Views>
class concat_view : view_interface<concat_view<Views...>> {
  static_assert(sizeof...(Views) > 0, "Constraints not satisfied");
  static_assert(conjunction<input_range<Views>...>::value, "Constraints not satisfied");
  static_assert(conjunction<view<Views>...>::value, "Constraints not satisfied");
  static_assert(detail::concatable<Views...>::value, "Constraints not satisfied");

  using difference_type_ = common_type_t<range_difference_t<Views>...>;
  using BackBase = typename type_sequence<Views...>::back;

  static constexpr std::size_t cranges = sizeof...(Views);
  template<template<typename, typename...> class Constraints>
  struct last_is : Constraints<std::tuple_element_t<cranges - 1, std::tuple<Views...>>> {};

  std::tuple<Views...> views_{};

  template<bool Const, bool AllForward /* = false */, bool HaveCommonRef>
  struct iterator_category_base {
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
    using iterator_category = iterator_ignore;
#endif
  };

  template<bool Const>
  struct iterator_category_base<Const, true, true> {
    using iterator_category = std::conditional_t<
        std::is_reference<typename detail::concat_reference<maybe_const<Const, Views>...>::type>::value,
        typename iterator_category_base<Const, true, false>::iterator_category,
        input_iterator_tag
    >;
  };

  template<bool Const, typename... Cs>
  struct iterator_category_base_2 {
    using iterator_category = conditional_t<
        conjunction<
            derived_from<Cs, random_access_iterator_tag>...,
            detail::concat_is_random_access<Const, Views...>
        >, random_access_iterator_tag,
        conjunction<
            derived_from<Cs, bidirectional_iterator_tag>...,
            detail::concat_is_bidirectional<Const, Views...>
        >, bidirectional_iterator_tag,
        conjunction<
            derived_from<Cs, forward_iterator_tag>...
        >, forward_iterator_tag,
        input_iterator_tag
    >;
  };

  template<bool Const>
  struct iterator_category_base<Const, true, false>
      : iterator_category_base_2<
          Const,
          typename iterator_traits<iterator_t<maybe_const<Const, Views>>>::iterator_category...
      > {};

 public:
  template<bool Const> struct iterator;
  template<bool Const> friend struct iterator;

  template<bool Const>
  struct iterator
      : iterator_category_base<
          Const,
          conjunction< maybe_const<Const, forward_range<Views>>... >::value,
          has_typename_type< detail::concat_reference<maybe_const<Const, Views>...> >::value
      >
  {
    using iterator_concept =
        conditional_t<
            conjunction<detail::concat_is_random_access<Const, Views>...>, random_access_iterator_tag,
            conjunction<detail::concat_is_bidirectional<Const, Views>...>, bidirectional_iterator_tag,
            conjunction<forward_range<maybe_const<Const, Views>>...>, forward_iterator_tag,
            input_iterator_tag
        >;
    using value_type = common_type_t<range_value_t<maybe_const<Const, Views>>...>;
    using difference_type = common_type_t<range_difference_t<maybe_const<Const, Views>>...>;

#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
    using reference = common_reference_t<range_reference_t<maybe_const<Const, Views>>...>;
    using pointer = void;
#endif

   private:
    friend struct iterator<!Const>;
    friend class concat_view;

    using base_iter = variant<iterator_t<maybe_const<Const, Views>>...>;
    using Parent = maybe_const<Const, concat_view>;

    Parent* parent_ = nullptr;
    base_iter it_;

    template<std::size_t N, std::enable_if_t<(N < cranges - 1), int> = 0>
    constexpr void satisfy() {
      using namespace preview::rel_ops;
      if (std::get<N>(it_) == ranges::end(std::get<N>(parent_->views_))) {
        it_.template emplace<N + 1>(ranges::begin(std::get<N + 1>(parent_->views_)));
        this->satisfy<N + 1>();
      }
    }

    template<std::size_t N, std::enable_if_t<(N >= cranges - 1), int> = 0>
    constexpr void satisfy() { /* no op */ }

    struct next_raw_visitor {
      iterator& self;

      template<typename I, std::size_t N>
      constexpr void operator()(I& it, in_place_index_t<N>) {
        ++preview::force_forward_like<iterator&>(it);
        self.satisfy<N>();
      }

      template<typename U> constexpr void operator()(U&, in_place_index_t<variant_npos>) { /* no op */ }
    };

    struct prev_raw_visitor {
      iterator& self;

      template<typename I>
      constexpr void operator()(I& it, in_place_index_t<0>) {
        --it;
      }

      template<typename I, std::size_t N>
      constexpr void operator()(I& it, in_place_index_t<N>) {
        if (it == ranges::begin(std::get<N>(self.parent_->views_))) {
          self.it_.template emplace<N - 1>(ranges::end(std::get<N - 1>(self.parent_->views_)));
          self.it_._visit_indexed_args(*this);
        } else {
          --it;
        }
      }

      template<typename U> constexpr void operator()(U&, in_place_index_t<variant_npos>) { /* no op */ }
    };

    template<bool Init>
    struct advance_fwd_raw_visitor {
      iterator& self;
      difference_type steps;

      template<typename I>
      constexpr void operator()(I& it, in_place_index_t<cranges - 1>) {
        using underlying_diff_type = iter_difference_t<variant_alternative_t<cranges - 1, base_iter>>;
        preview::force_forward_like<iterator&>(it) += static_cast<underlying_diff_type>(steps);
      }

      template<typename I, std::size_t N, std::enable_if_t<(Init && N != variant_npos), int> = 0>
      constexpr void operator()(I& it, in_place_index_t<N>) {
        using underlying_diff_type = iter_difference_t<variant_alternative_t<N, base_iter>>;
        const difference_type offset = it - ranges::begin(std::get<N>(self.parent_->views_));

        auto n_size = ranges::distance(std::get<N>(self.parent_->views_));
        if (offset + steps < n_size) {
          preview::force_forward_like<iterator&>(it) += static_cast<underlying_diff_type>(steps);
        } else {
          self.it_.template emplace<N + 1>(ranges::begin(std::get<N + 1>(self.parent_->views_)));
          self.it_._visit_indexed_args(advance_fwd_raw_visitor<false>{self, offset + steps - n_size});
        }
      }

      // offset is 0
      template<typename I, std::size_t N, std::enable_if_t<(!Init && N != variant_npos), int> = 0>
      constexpr void operator()(I& it, in_place_index_t<N>) {
        using underlying_diff_type = iter_difference_t<variant_alternative_t<N, base_iter>>;

        auto n_size = ranges::distance(std::get<N>(self.parent_->views_));
        if (steps < n_size) {
          preview::force_forward_like<iterator&>(it) += static_cast<underlying_diff_type>(steps);
        } else {
          self.it_.template emplace<N + 1>(ranges::begin(std::get<N + 1>(self.parent_->views_)));
          self.it_._visit_indexed_args(advance_fwd_raw_visitor<false>{self, steps - n_size});
        }
      }

      template<typename U> constexpr void operator()(U&, in_place_index_t<variant_npos>) { /* no op */ }
    };

    template<bool Init>
    struct advance_bwd_raw_visitor {
      iterator* pos;
      difference_type steps;

      template<typename I, typename... Offset>
      constexpr void operator()(I& it, in_place_index_t<0>, Offset...) {
        using underlying_diff_type = iter_difference_t<variant_alternative_t<0, base_iter>>;
        it -= static_cast<underlying_diff_type>(steps);
      }

      template<typename I, std::size_t N, std::enable_if_t<Init, int> = 0>
      constexpr void operator()(I& it, in_place_index_t<N>) {
        using underlying_diff_type = iter_difference_t<variant_alternative_t<N, base_iter>>;
        const difference_type offset = it - ranges::begin(std::get<N>(pos->parent_->views_));

        if (offset >= steps) {
          it -= static_cast<underlying_diff_type>(steps);
        } else {
          auto prev_size = ranges::distance(std::get<N - 1>(pos->parent_->views_));
          pos->it_.template emplace<N - 1>(ranges::end(std::get<N - 1>(pos->parent_->views_)));
          pos->it_._visit_indexed_args(advance_bwd_raw_visitor<false>{pos, steps - offset}, prev_size);
        }
      }

      template<typename I, std::size_t N, std::enable_if_t<!Init, int> = 0>
      constexpr void operator()(I& it, in_place_index_t<N>, difference_type offset) {
        using underlying_diff_type = iter_difference_t<variant_alternative_t<N, base_iter>>;
        if (offset >= steps) {
          it -= static_cast<underlying_diff_type>(steps);
        } else {
          auto prev_size = ranges::distance(std::get<N - 1>(pos->parent_->views_));
          pos->it_.template emplace<N - 1>(ranges::end(std::get<N - 1>(pos->parent_->views_)));
          pos->it_._visit_indexed_args(advance_bwd_raw_visitor<false>{pos, steps - offset}, prev_size);
        }
      }

      template<typename U, typename... Offset> constexpr void operator()(U&, in_place_index_t<variant_npos>, Offset...) { /* no op */ }
    };

    struct distance_raw_visitor {
      const iterator& x;
      const iterator& y;

      template<typename I, std::size_t IX, typename Dummy, std::enable_if_t<(IX != variant_npos), int> = 0>
      constexpr difference_type operator()(const I& i, in_place_index_t<IX> ix, Dummy, in_place_index_t<variant_npos>) const {
        return y.it_._visit_indexed_args(*this, i, in_place_index<IX>);
      }

      template<typename J, std::size_t JX, typename I, std::size_t IX, std::enable_if_t<(JX != variant_npos && IX != variant_npos && IX > JX), int> = 0>
      constexpr difference_type operator()(const J& j, in_place_index_t<JX> jx, const I& i, in_place_index_t<IX> ix) const {
        return calculate(x, i, in_place_index<IX>, y, j, in_place_index<JX>);
      }

      template<typename J, std::size_t JX, typename I, std::size_t IX, std::enable_if_t<(JX != variant_npos && IX != variant_npos && IX < JX), int> = 0>
      constexpr difference_type operator()(const J& j, in_place_index_t<JX> jx, const I& i, in_place_index_t<IX> ix) const {
        return -calculate(y, j, in_place_index<JX>, x, i, in_place_index<IX>);
      }

      template<typename J, std::size_t JX, typename I, std::size_t IX, std::enable_if_t<(JX != variant_npos && IX != variant_npos && IX == JX), int> = 0>
      constexpr difference_type operator()(const J& j, in_place_index_t<JX> jx, const I& i, in_place_index_t<IX> ix) const {
        return i - j;
      }

      template<typename U, typename... IY>
      constexpr difference_type operator()(const U&, in_place_index_t<variant_npos>, IY...) const {
        // unreachable
        return static_cast<difference_type>(variant_npos);
      }

     private:
      template<typename I, std::size_t IX, typename J, std::size_t JX>
      static constexpr difference_type calculate(const iterator& x, const I& i, in_place_index_t<IX> ix,
                                                 const iterator& y, const J& j, in_place_index_t<JX> jx)
      {
        static_assert(IX > JX, "Invalid index");

        const auto dy = ranges::distance(j, ranges::end(std::get<JX>(y.parent_->views_)));
        const auto dx = ranges::distance(ranges::begin(std::get<IX>(x.parent_->views_)), i);
        const difference_type s = preview::detail::tuple_fold_left_impl<JX + 1, IX>(
            x.parent_->views_,
            difference_type(0),
            [](auto acc, auto&& v) {
              return acc + static_cast<difference_type>(ranges::size(v));
            }
        );
        return dy + s + dx;
      }
    };

    struct distance_default_raw_visitor {
      const iterator& x;

      template<typename I, std::size_t IX>
      constexpr difference_type operator()(const I& i, in_place_index_t<IX>) const {
        const auto d = ranges::distance(i, ranges::end(std::get<IX>(x.parent_->views_)));
        const difference_type s = preview::detail::tuple_fold_left_impl<IX + 1, cranges>(
            x.parent_->views_,
            difference_type(0),
            [](auto acc, auto&& v) {
              return acc + static_cast<difference_type>(ranges::size(v));
            }
        );
        return -(d + s);
      }

      template<typename U>
      constexpr difference_type operator()(const U&, in_place_index_t<variant_npos>) const {
        // unreachable
        return static_cast<difference_type>(variant_npos);
      }
    };

    struct variant_const_visitor {
      template<typename I, std::size_t N>
      constexpr base_iter operator()(I&& x, in_place_index_t<N>) {
        return base_iter{in_place_index<N>, std::forward<I>(x)};
      }

      template<typename U>
      constexpr void operator()(U&, in_place_index_t<variant_npos>) { /* no op */ }
    };

    struct iter_swap_visitor {
      template<typename I1, typename I2, std::enable_if_t<std::is_same<I1, I2>::value, int> = 0>
      void operator()(const I1& x, const I2& y) const {
        ranges::iter_swap(x, y);
      }

      template<typename I1, typename I2, std::enable_if_t<!std::is_same<I1, I2>::value, int> = 0>
      void operator()(const I1& x, const I2& y) const {
        ranges::swap(*x, *y);
      }
    };

    constexpr bool equal_with_default_sentinel() const {
      return it_.index() == cranges - 1 &&
             std::get<cranges - 1>(it_) == ranges::end(std::get<cranges - 1>(parent_->views_));
    }

   public:
    iterator() = default;

    template<std::size_t I, typename It>
    constexpr iterator(Parent* parent, in_place_index_t<I>, It&& it)
      : parent_(parent)
      , it_{in_place_index<I>, std::forward<It>(it)} {}

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<(AntiConst != Const && Const)>,
        convertible_to<iterator_t<Views>, iterator_t<const Views>>...
    >::value, int> = 0>
    constexpr iterator(iterator<AntiConst> it)
      : parent_(it.parent_)
      , it_(std::move(it.it_).visit(variant_const_visitor{})) {}

    constexpr decltype(auto) operator*() const {
      using ref = typename detail::concat_reference<maybe_const<Const, Views>...>::type;
      return it_.visit([](auto&& it) -> ref { return *it; });
    }

    constexpr iterator& operator++() {
      it_._visit_indexed_args(next_raw_visitor{*this});
      return *this;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        negation<forward_range<maybe_const<Const, Views>>>...
    >::value, int> = 0>
    constexpr void operator++(int) {
      ++*this;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        forward_range<maybe_const<Const, Views>>...
    >::value, int> = 0>
    constexpr iterator operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        detail::concat_is_bidirectional<Const, Views...>
    >::value, int> = 0>
    constexpr iterator& operator--() {
      it_._visit_indexed_args(prev_raw_visitor{*this});
      return *this;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        detail::concat_is_bidirectional<Const, Views...>
    >::value, int> = 0>
    constexpr iterator operator--(int) {
      auto tmp = *this;
      --*this;
      return tmp;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        detail::concat_is_random_access<Const, Views...>
    >::value, int> = 0>
    constexpr iterator& operator+=(difference_type n) {
      if (n > 0) {
        it_._visit_indexed_args(advance_fwd_raw_visitor<true>{*this, n});
      } else if (n < 0) {
        it_._visit_indexed_args(advance_bwd_raw_visitor<true>{this, n});
      }
      return *this;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        detail::concat_is_random_access<Const, Views...>
    >::value, int> = 0>
    constexpr iterator& operator-=(difference_type n) {
      *this += -n;
      return *this;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        detail::concat_is_random_access<Const, Views...>
    >::value, int> = 0>
    constexpr decltype(auto) operator[](difference_type n) const {
      return *((*this) + n);
    }

    template<typename V = maybe_const<Const, base_iter>, std::enable_if_t<equality_comparable<V>::value, int> = 0>
    friend constexpr bool operator==(const iterator& x, const iterator& y) {
      return x.it_ == y.it_;
    }

    template<typename V = maybe_const<Const, base_iter>, std::enable_if_t<equality_comparable<V>::value, int> = 0>
    friend constexpr bool operator!=(const iterator& x, const iterator& y) {
      return !(x == y);
    }

    friend constexpr bool operator==(const iterator& x, default_sentinel_t) {
      return x.equal_with_default_sentinel();
    }

    friend constexpr bool operator!=(const iterator& x, default_sentinel_t) {
      return !x.equal_with_default_sentinel();
    }

    friend constexpr bool operator==(default_sentinel_t, const iterator& x) {
      return x.equal_with_default_sentinel();
    }

    friend constexpr bool operator!=(default_sentinel_t, const iterator& x) {
      return !x.equal_with_default_sentinel();
    }

    template<bool B = conjunction<random_access_range<maybe_const<Const, Views>>...>::value, std::enable_if_t<B, int> = 0>
    friend constexpr bool operator<(const iterator& x, const iterator& y) {
      return x.it_ < y.it_;
    }
    template<bool B = conjunction<random_access_range<maybe_const<Const, Views>>...>::value, std::enable_if_t<B, int> = 0>
    friend constexpr bool operator>(const iterator& x, const iterator& y) {
      return x.it_ > y.it_;
    }
    template<bool B = conjunction<random_access_range<maybe_const<Const, Views>>...>::value, std::enable_if_t<B, int> = 0>
    friend constexpr bool operator<=(const iterator& x, const iterator& y) {
      return x.it_ <= y.it_;
    }
    template<bool B = conjunction<random_access_range<maybe_const<Const, Views>>...>::value, std::enable_if_t<B, int> = 0>
    friend constexpr bool operator>=(const iterator& x, const iterator& y) {
      return x.it_ >= y.it_;
    }

    template<bool B = detail::concat_is_random_access<Const, Views...>::value, std::enable_if_t<B, int> = 0>
    friend constexpr iterator operator+(const iterator& it, difference_type n) {
      auto temp = it;
      temp += n;
      return temp;
    }

    template<bool B = detail::concat_is_random_access<Const, Views...>::value, std::enable_if_t<B, int> = 0>
    friend constexpr iterator operator+(difference_type n, const iterator& it) {
      return it + n;
    }

    template<bool B = detail::concat_is_random_access<Const, Views...>::value, std::enable_if_t<B, int> = 0>
    friend constexpr difference_type operator-(const iterator& x, const iterator& y) {
      return x.it_._visit_indexed_args(distance_raw_visitor{x, y}, in_place_index<variant_npos>, in_place_index<variant_npos>);
    }

    template<bool B = detail::concat_is_sized<Const, Views...>::value, std::enable_if_t<B, int> = 0>
    friend constexpr difference_type operator-(const iterator& x, default_sentinel_t) {
      return x.it_._visit_indexed_args(distance_default_raw_visitor{x});
    }

    template<bool B = detail::concat_is_sized<Const, Views...>::value, std::enable_if_t<B, int> = 0>
    friend constexpr difference_type operator-(default_sentinel_t, const iterator& x) {
      return -(x - default_sentinel);
    }

    friend constexpr decltype(auto) iter_move(const iterator& i)
        noexcept(conjunction<
            is_nothrow_invocable< decltype(ranges::iter_move), const iterator_t<maybe_const<Const, Views>>& >...,
            is_nothrow_convertible< range_rvalue_reference_t< maybe_const<Const, Views> >,
                                    detail::concat_rvalue_reference< maybe_const<Const, Views>... > >...
        >::value)
    {
      return preview::visit(
          [](const auto& i) -> typename detail::concat_rvalue_reference<maybe_const<Const, Views>...>::type {
            return ranges::iter_move(i);
          },
          i.it_
      );
    }

    template<bool B = conjunction<
        swappable_with<iter_reference_t<iterator>, iter_reference_t<iterator>>,
        indirectly_swappable<iterator_t<maybe_const<Const, Views>>>...
    >::value, std::enable_if_t<B, int> = 0>
    friend constexpr void iter_swap(const iterator& x, const iterator& y)
        noexcept(conjunction<
            bool_constant<noexcept(ranges::swap(*x, *y))>,
            bool_constant<noexcept(ranges::iter_swap(std::declval<const iterator_t<maybe_const<Const, Views>>&>(),
                                                     std::declval<const iterator_t<maybe_const<Const, Views>>&>()))>...
        >::value)
    {
      preview::visit(iter_swap_visitor{}, x.it_, y.it_);
    }
  };

  concat_view() = default;

  explicit concat_view(Views... views)
      : views_{std::move(views)...} {}

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      negation<conjunction<simple_view<Views>...>>
  >::value, int> = 0>
  constexpr iterator<false> begin() {
    iterator<false> it{this, in_place_index<0>, ranges::begin(std::get<0>(views_))};
    it.template satisfy<0>();
    return it;
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      range<const Views>...,
      detail::concatable<const Views...>
  >::value, int> = 0>
  constexpr iterator<true> begin() const {
    iterator<true> it{this, in_place_index<0>, ranges::begin(std::get<0>(views_))};
    it.template satisfy<0>();
    return it;
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      negation<conjunction<simple_view<Views>...>>
  >::value, int> = 0>
  constexpr auto end() {
    return end_impl(*this, last_is<common_range>{}, std::false_type{});
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      range<const Views>...,
      detail::concatable<const Views...>
  >::value, int> = 0>
  constexpr auto end() const {
    return end_impl(*this, last_is<common_range>{}, std::true_type{});
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      sized_range<Views>...
  >::value, int> = 0>
  constexpr auto size() {
#if PREVIEW_CXX_VERSION < 17
    using CT = common_type_t<decltype(ranges::size(std::declval<Views&>()))...>;
    using UCT = make_unsigned_like_t<CT>;

    return preview::tuple_fold_left(
        preview::tuple_transform(views_, ranges::size),
        UCT(0),
        [](auto x, auto y) { return static_cast<UCT>(x) + static_cast<UCT>(y); }
    );
#else
    return preview::apply(
        [](auto... sizes) {
          using CT = common_type_t<decltype(sizes)...>;
          using UCT = make_unsigned_like_t<CT>;
          return (static_cast<UCT>(sizes) + ...);
        },
        preview::tuple_transform(views_, ranges::size)
    );
#endif
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      sized_range<const Views>...
  >::value, int> = 0>
  constexpr auto size() const {
#if PREVIEW_CXX_VERSION < 17
    using CT = common_type_t<decltype(ranges::size(std::declval<const Views&>()))...>;
    using UCT = make_unsigned_like_t<CT>;

    return preview::tuple_fold_left(
        preview::tuple_transform(views_, ranges::size),
        UCT(0),
        [](auto x, auto y) { return static_cast<UCT>(x) + static_cast<UCT>(y); }
    );
#else
    return preview::apply(
        [](auto... sizes) {
          using CT = common_type_t<decltype(sizes)...>;
          using UCT = make_unsigned_like_t<CT>;
          return (static_cast<UCT>(sizes) + ...);
        },
        preview::tuple_transform(views_, ranges::size)
    );
#endif
  }

 private:
  template<typename Self, bool Const>
  static constexpr auto end_impl(Self& self, std::true_type, bool_constant<Const>) {
    iterator<Const> it{preview::addressof(self), in_place_index<cranges - 1>, ranges::end(std::get<cranges - 1>(self.views_))};
    return it;
  }

  template<typename Self, bool Const>
  static constexpr auto end_impl(Self&, std::false_type, bool_constant<Const>) {
    return default_sentinel;
  }
};

#if __cplusplus >= 201703L

template<typename... R>
concat_view(R&&...) -> concat_view<views::all_t<R>...>;

#endif

namespace views {
namespace detail {

struct concat_niebloid {
  template<typename... R, std::enable_if_t<conjunction<
      viewable_range<R>...,
      input_range<R>...
  >::value, int> = 0>
  constexpr auto operator()(R&&... rs) const {
    return concat_view<all_t<R>...>{views::all(std::forward<R>(rs))...};
  }
};

} // namespace detail

constexpr PREVIEW_INLINE_VARIABLE detail::concat_niebloid concat{};

} // namespace views

} // namespace ranges
} // namespace preview

#endif //CONCAT_VIEW_H_
