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
#include "preview/__core/maybe_unused.h"
#include "preview/__core/static_assert.h"
#include "preview/config.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__concepts/default_initializable.h"
#include "preview/__concepts/dereferenceable.h"
#include "preview/__contract/specifiers.h"
#include "preview/__iterator/default_sentinel_t.h"
#include "preview/__iterator/next.h"
#include "preview/__iterator/iter_move.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__ranges/common_range.h"
#include "preview/__ranges/simple_view.h"
#include "preview/__ranges/distance.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/range_difference_t.h"
#include "preview/__ranges/range_reference_t.h"
#include "preview/__ranges/range_rvalue_reference_t.h"
#include "preview/__ranges/range_value_t.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/helpers.h"
#include "preview/__tuple/apply.h"
#include "preview/__tuple/tuple_fold.h"
#include "preview/__tuple/tuple_transform.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/common_type.h"
#include "preview/__type_traits/common_reference.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/index_constant.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/make_unsigned_like.h"
#include "preview/__type_traits/maybe_const.h"
#include "preview/__utility/type_sequence.h"
#include "preview/variant.h"

namespace preview {
namespace ranges {
namespace detail {

template<bool B /* typename range_reference_t<Rs>... = false */, typename... Rs>
struct concat_reference_impl {};
template<typename... Rs>
struct concat_reference_impl<true, Rs...> : common_reference<range_reference_t<Rs>...> {};
template<typename... Rs>
struct concat_reference : concat_reference_impl<conjunction_v< has_typename_type<range_reference<Rs>>... >, Rs...> {};
template<typename... Rs>
using concat_reference_t = typename concat_reference<Rs...>::type;

template<bool B /* typename range_value_t<Rs>... = false */, typename... Rs>
struct concat_value_impl {};
template<typename... Rs>
struct concat_value_impl<true, Rs...> : common_type<range_value_t<Rs>...> {};
template<typename... Rs>
struct concat_value : concat_value_impl<conjunction_v< has_typename_type<range_value<Rs>>... >, Rs...> {};
template<typename... Rs>
using concat_value_t = typename concat_value<Rs...>::type;

template<bool B /* typename range_rvalue_reference_t<Rs>... = false */, typename... Rs>
struct concat_rvalue_reference_impl {};
template<typename... Rs>
struct concat_rvalue_reference_impl<true, Rs...> : common_reference<range_rvalue_reference_t<Rs>...> {};
template<typename... Rs>
struct concat_rvalue_reference : concat_rvalue_reference_impl<conjunction_v< has_typename_type<range_rvalue_reference<Rs>>... >, Rs...> {};
template<typename... Rs>
using concat_rvalue_reference_t = typename concat_rvalue_reference<Rs...>::type;

template<typename Ref, typename RRef, typename It,
    bool = /* false */ conjunction_v<dereferenceable<It>, is_invocable<decltype(ranges::iter_move), const It&>>>
struct concat_indirectly_readable_impl : std::false_type {};

template<typename Ref, typename RRef, typename It>
struct concat_indirectly_readable_impl<Ref, RRef, It, true> : conjunction<
    convertible_to<decltype(*std::declval<const It&>()), Ref>,
    convertible_to<decltype(ranges::iter_move(std::declval<const It&>())), RRef>
> {};

template<typename... Rs>
struct concat_indirectly_readable : conjunction<
    common_reference_with<concat_reference       <Rs...>&&, concat_value           <Rs...>&      >,
    common_reference_with<concat_reference       <Rs...>&&, concat_rvalue_reference<Rs...>&&     >,
    common_reference_with<concat_rvalue_reference<Rs...>&&, concat_value           <Rs...> const&>,
    concat_indirectly_readable_impl<concat_reference_t<Rs...>,
                                    concat_rvalue_reference_t<Rs...>,
                                    iterator_t<Rs>>...
>{};

template<bool B /* false */, typename... Rs>
struct concatable_impl : std::false_type {};
template<typename... Rs>
struct concatable_impl<true, Rs...> : std::true_type{}; // concat_indirectly_readable<Rs...> {};
template<typename... Rs>
struct concatable : concatable_impl<
    conjunction_v<
        has_typename_type<concat_reference<Rs...>>,
        has_typename_type<concat_value<Rs...>>,
        has_typename_type<concat_rvalue_reference<Rs...>>
    >,
    Rs...
> {};

template<bool Const, typename... Rs>
struct concat_is_random_access;
template<bool Const, typename R>
struct concat_is_random_access<Const, R> : all_random_access<Const, R> {};
template<bool Const, typename R, typename... Rs>
struct concat_is_random_access<Const, R, Rs...> : conjunction<
    all_random_access<Const, R>,
    common_range<maybe_const<Const, R>>,
    concat_is_random_access<Const, Rs...>
> {};

template<bool Const, typename... Rs>
struct concat_is_bidirectional;
template<bool Const, typename R>
struct concat_is_bidirectional<Const, R> : all_bidirectional<Const, R> {};
template<bool Const, typename R, typename... Rs>
struct concat_is_bidirectional<Const, R, Rs...> : conjunction<
    all_bidirectional<Const, R>,
    common_range<maybe_const<Const, R>>,
    concat_is_bidirectional<Const, Rs...>
> {};

template<bool Const, typename... Views>
struct concat_iterator_category_impl {
  template<typename View>
  using C = typename iterator_traits< iterator_t<maybe_const<Const, View>> >::iterator_category;

  using type = conditional_t<
      conjunction<derived_from<C<Views>, random_access_iterator_tag>..., concat_is_random_access<Const, Views...>>, random_access_iterator_tag,
      conjunction<derived_from<C<Views>, bidirectional_iterator_tag>..., concat_is_bidirectional<Const, Views...>>, bidirectional_iterator_tag,
      conjunction<derived_from<C<Views>, forward_iterator_tag      >...                                          >, forward_iterator_tag,
      input_iterator_tag
  >;
};

template<bool B /* all_forward = false */, bool Const, typename... Views>
struct concat_iterator_category {};
template<bool Const, typename... Views>
struct concat_iterator_category<true, Const, Views...> {
  using iterator_category = std::conditional_t<
      std::is_reference<concat_reference_t<maybe_const<Const, Views>...>>::value == false,
      input_iterator_tag,
      typename concat_iterator_category_impl<Const, Views...>::type
  >;
};

} // namespace detail

template<typename... Views>
class concat_view : public view_interface<concat_view<Views...>> {
  static_assert(conjunction_v<input_range<Views>...>, "Constraints not satisfied");
  static_assert(conjunction_v<view<Views>...>,        "Constraints not satisfied");
  static_assert(sizeof...(Views) != 0,                "Constraints not satisfied");
  static_assert(detail::concatable<Views...>::value,  "Constraints not satisfied");

 private:
  using difference_type_ = common_type_t<range_difference_t<Views>...>;
  using BackBase = typename type_sequence<Views...>::back;

  static constexpr std::size_t cranges = sizeof...(Views);
  std::tuple<Views...> views_;



 public:
  template<bool Const>
  struct iterator;

  template<bool Const>
  struct iterator
      : detail::concat_iterator_category<detail::all_forward<Const, Views...>::value, Const, Views...>
  {
    friend class concat_view;

    using iterator_concept =
        conditional_t<
            detail::concat_is_random_access<Const, Views...>, random_access_iterator_tag,
            detail::concat_is_bidirectional<Const, Views...>, bidirectional_iterator_tag,
            detail::all_forward<Const, Views...>, forward_iterator_tag,
            input_iterator_tag
        >;
    using value_type = detail::concat_value_t<maybe_const<Const, Views>...>;
    using difference_type = common_type_t<range_difference_t<maybe_const<Const, Views>>...>;

#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
    using reference = common_reference_t<range_reference_t<maybe_const<Const, Views>>...>;
    using pointer = void;
#endif

   private:
    friend struct iterator<!Const>;

    using base_iter = variant<iterator_t<maybe_const<Const, Views>>...>;
    using Parent = maybe_const<Const, concat_view>;

    Parent* parent_ = nullptr;
    base_iter it_;

    template<std::size_t N, typename I, std::enable_if_t<(N < sizeof...(Views) - 1), int> = 0>
    constexpr void satisfy(I& it) {
      if (it == ranges::end(std::get<N>(parent_->views_))) {
        auto& current = it_.template emplace<N + 1>(ranges::begin(std::get<N + 1>(parent_->views_)));
        satisfy<N + 1>(current);
      }
    }

    template<std::size_t N, typename I, std::enable_if_t<(N >= sizeof...(Views) - 1), int> = 0>
    constexpr void satisfy(I&) { /* no op */ }

    struct next_raw_visitor {
      iterator* pos;

      template<typename I, std::size_t N>
      void operator()(I&& it, in_place_index_t<N>) {
        ++it;
        pos->template satisfy<N>(it);
      }
      template<typename T> constexpr void operator()(T&&, in_place_index_t<variant_npos>) { /* no op */ }
    };

    template<std::size_t N, typename I, std::enable_if_t<(N == 0), int> = 0>
    constexpr void prev(I& it) {
      --it;
    }

    template<std::size_t N, typename I, std::enable_if_t<(N > 0), int> = 0>
    constexpr void prev(I& it) {
      if (it == ranges::begin(std::get<N>(parent_->views_))) {
        auto& current = it_.template emplace<N - 1>(ranges::end(std::get<N - 1>(parent_->views_)));
        prev<N - 1>(current);
      } else {
        --it;
      }
    }

    struct prev_raw_visitor {
      iterator* pos;

      template<typename I, std::size_t N>
      constexpr void operator()(I& it, in_place_index_t<N>) {
        pos->prev<N>(it);
      }
      template<typename T> constexpr void operator()(T&&, in_place_index_t<variant_npos>) { /* no op */ }
    };

    template<std::size_t N, typename I, std::enable_if_t<(N == sizeof...(Views) - 1), int> = 0>
    constexpr void advance_fwd(I& it, difference_type /* offset */, difference_type steps) {
      using underlying_diff_type = iter_difference_t<variant_alternative_t<N, base_iter>>;
      it += static_cast<underlying_diff_type>(steps);
    }

    template<std::size_t N, typename I, std::enable_if_t<(N != sizeof...(Views) - 1), int> = 0>
    constexpr void advance_fwd(I& it, difference_type offset, difference_type steps) {
      using underlying_diff_type = iter_difference_t<variant_alternative_t<N, base_iter>>;
      auto n_size = ranges::distance(std::get<N>(parent_->views_));
      if (offset + steps < n_size) {
        it += static_cast<underlying_diff_type>(steps);
      } else {
        auto& current = it_.template emplace<N + 1>(ranges::begin(std::get<N + 1>(parent_->views_)));
        advance_fwd<N + 1>(current, 0, offset + steps - n_size);
      }
    }

    struct advance_fwd_raw_visitor {
      iterator* pos;
      difference_type steps;

      template<typename I, std::size_t i>
      constexpr void operator()(I& it, in_place_index_t<i>) {
        pos->advance_fwd<i>(it, it - ranges::begin(std::get<i>(pos->parent_->views_)), steps);
      }
      template<typename T> constexpr void operator()(T&&, in_place_index_t<variant_npos>) { /* no op */ }
    };

    template<std::size_t N, typename I, std::enable_if_t<(N == 0), int> = 0>
    constexpr void advance_bwd(I& it, difference_type /* offset */, difference_type steps) {
      using underlying_diff_type = iter_difference_t<variant_alternative_t<N, base_iter>>;
      it -= static_cast<underlying_diff_type>(steps);
    }

    template<std::size_t N, typename I, std::enable_if_t<(N > 0), int> = 0>
    constexpr void advance_bwd(I& it, difference_type offset, difference_type steps) {
      using underlying_diff_type = iter_difference_t<variant_alternative_t<N, base_iter>>;
      if (offset >= steps) {
        it -= static_cast<underlying_diff_type>(steps);
      } else {
        auto prev_size = ranges::distance(std::get<N - 1>(parent_->views_));
        auto& current = it_.template emplace<N - 1>(ranges::end(std::get<N - 1>(parent_->views_)));
        advance_bwd<N - 1>(current, prev_size, steps - offset);
      }
    }

    struct advance_bwd_raw_visitor {
      iterator* pos;
      difference_type steps;

      template<typename I, std::size_t i>
      constexpr void operator()(I& it, in_place_index_t<i>) {
        pos->advance_bwd<i>(it, it - ranges::end(std::get<i>(pos->parent_->views_)), steps);
      }
      template<typename T> constexpr void operator()(T&&, in_place_index_t<variant_npos>) { /* no op */ }
    };

    static difference_type distance_to_(std::integral_constant<size_t, cranges>, iterator const &, iterator const &) {
      // unreachable
      return -1;
    }

    template<std::size_t N>
    static difference_type distance_to_(std::integral_constant<size_t, N>, const iterator& from, const iterator& to) {
      if (from.it_.index() > N)
        return iterator::distance_to_(std::integral_constant<size_t, N + 1>{}, from, to);

      if (from.it_.index() == N) {
        if (to.it_.index() == N)
          return ranges::distance(preview::detail::variant_raw_get(from.it_._base().storage(), in_place_index<N>),
                                  preview::detail::variant_raw_get(to.it_._base().storage(), in_place_index<N>));
        return ranges::distance(preview::detail::variant_raw_get(from.it_._base().storage(), in_place_index<N>),
                                ranges::end(std::get<N>(from.parent_->views_)))
               + iterator::distance_to_(std::integral_constant<size_t, N + 1>{}, from, to);
      }

      if (from.it_.index() < N && to.it_.index() > N)
          return ranges::distance(std::get<N>(from.parent_->views_))
              + iterator::distance_to_(std::integral_constant<size_t, N + 1>{}, from, to);

      return ranges::distance(ranges::begin(std::get<N>(from.parent_->views_)),
                              preview::detail::variant_raw_get(to.it_._base().storage(), in_place_index<N>));
    }

    template<std::size_t I, typename... Args, std::enable_if_t<conjunction_v<constructible_from<base_iter, in_place_index_t<I>, Args&&>...>, int> = 0>
    constexpr explicit iterator(Parent* parent, in_place_index_t<I>, Args&&... args)
        : parent_(parent)
        , it_(in_place_index<I>, std::forward<Args>(args)...) {}

    template<typename... Args, std::enable_if_t<conjunction_v<constructible_from<base_iter, Args&&>...>, int> = 0>
    constexpr explicit iterator(Parent* parent, Args&&... args)
        : parent_(parent)
        , it_(std::forward<Args>(args)...) {}

    constexpr iterator post_increment(std::true_type /* all_forward */) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    constexpr void post_increment(std::false_type /* all_forward */) {
      ++*this;
    }

   public:
    iterator() = default;

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<(AntiConst != Const && Const)>,
        convertible_to<iterator_t<Views>, iterator_t<const Views>>...
    >::value, int> = 0>
    constexpr iterator(iterator<AntiConst> it)
        preview_pre(it.it_.valueless_by_exception() == false)
        : parent_(it.parent_)
        , it_(preview::detail::variant_valueless_ctor{})
    {
      it.it_._visit_raw(preview::detail::variant_ctor_raw_visitor<iterator_t<maybe_const<Const, Views>>...>{it_._base()});
    }

    constexpr decltype(auto) operator*() const
        preview_pre(it_.valueless_by_exception() == false)
    {
      return it_.visit([](auto&& it) -> detail::concat_reference_t<maybe_const<Const, Views>...> { return *it; });
    }

    template<bool B = conjunction_v<equality_comparable< iterator_t<maybe_const<Const, Views>> >...>, std::enable_if_t<B, int> = 0>
    friend constexpr bool operator==(const iterator& x, const iterator& y)
        preview_pre(x.valueless_by_exception() == false && y.valueless_by_exception() == false)
    {
      return x.it_ == y.it_;
    }

    friend constexpr bool operator==(const iterator& it, default_sentinel_t)
        preview_pre(it_.valueless_by_exception() == false)
    {
      constexpr auto last_idx = sizeof...(Views) - 1;
      return it.it_.index() == last_idx &&
             preview::detail::variant_raw_get(it.it_._base().storage(), in_place_index<last_idx>) == ranges::end(std::get<last_idx>(it.parent_->views_));
    }

#if !PREVIEW_CONFORM_CXX20_STANDARD
    friend constexpr bool operator==(default_sentinel_t, const iterator& it) {
      return it == default_sentinel;
    }
#endif

    template<typename V = variant<iterator_t<maybe_const<Const, Views>>...>, std::enable_if_t<
        equality_comparable<V>
    ::value, int> = 0>
    friend constexpr bool operator!=(const iterator& x, const iterator& y) {
      return !(x == y);
    }

    constexpr iterator& operator++() {
      preview::detail::variant_raw_visit(it_.index(), it_._base().storage(), next_raw_visitor{this});
      return *this;
    }

    constexpr std::conditional_t<detail::all_forward<Const, Views...>::value, iterator, void> operator++(int) {
      return post_increment(detail::all_forward<Const, Views...>{});
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<
        std::is_void<Dummy>,
        detail::concat_is_bidirectional<Const, Views...>
    >::value, int> = 0>
    constexpr iterator& operator--()
        preview_pre(it_.valueless_by_exception() == false)
    {
      it_._visit_raw(prev_raw_visitor{this});
      return *this;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<
        std::is_void<Dummy>,
        bidirectional_range<Views>...
    >::value, int> = 0>
    constexpr iterator operator--(int) {
      auto tmp = *this;
      --*this;
      return tmp;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<
        std::is_void<Dummy>,
        detail::concat_is_random_access<Const, Views...>
    >::value, int> = 0>
    constexpr iterator& operator+=(difference_type n)
        preview_pre(it_.valueless_by_exception() == false)
    {
      if (n > 0) {
        it_._visit_raw(advance_fwd_raw_visitor{this, n});
      } else if (n < 0) {
        it_._visit_raw(advance_bwd_raw_visitor{this, -n});
      }
      return *this;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<
        std::is_void<Dummy>,
        detail::concat_is_random_access<Const, Views...>
    >::value, int> = 0>
    constexpr iterator& operator-=(difference_type n) {
      *this += -n;
      return *this;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<
        std::is_void<Dummy>,
        detail::concat_is_random_access<Const, Views...>
    >::value, int> = 0>
    constexpr decltype(auto) operator[](difference_type n) const {
      return *((*this) + n);
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<
        std::is_void<Dummy>,
        sized_sentinel_for<iterator_t<Views>, iterator_t<Views>>...
    >::value, int> = 0>
    friend constexpr difference_type operator-(const iterator& x, const iterator& y) {
      if (x.it_.index() <= y.it_.index())
        return -iterator::distance_to_(index_constant<0>{}, x, y);
      return iterator::distance_to_(index_constant<0>{}, y, x);
    }
  };

  constexpr concat_view() = default;

  explicit concat_view(Views... views)
      : views_(std::move(views)...) {}

  template<bool B = !conjunction_v<simple_view<Views>...>, std::enable_if_t<B, int> = 0>
  constexpr iterator<false> begin() {
    iterator<false> it{this, in_place_index<0>, ranges::begin(std::get<0>(views_))};
    it.template satisfy<0>(preview::detail::variant_raw_get(it.it_._base().storage(), in_place_index<0>));
    return it;
  }

  template<bool B = conjunction_v<range<const Views>..., detail::concatable<const Views...>>, std::enable_if_t<B, int> = 0>
  constexpr iterator<true> begin() const {
    iterator<true> it{this, in_place_index<0>, ranges::begin(std::get<0>(views_))};
    it.template satisfy<0>(preview::detail::variant_raw_get(it.it_._base().storage(), in_place_index<0>));
    return it;
  }

  template<bool B = !conjunction_v<simple_view<Views>...>, std::enable_if_t<B, int> = 0>
  constexpr auto end() {
    using last_view_type = std::tuple_element_t<cranges - 1, std::tuple<Views...>>;
    return end_impl<false>(this, common_range<last_view_type>{});
  }

  template<bool B = conjunction_v<range<const Views>..., detail::concatable<const Views...>>, std::enable_if_t<B, int> = 0>
  constexpr auto end() const {
    using last_view_type = std::tuple_element_t<cranges - 1, std::tuple<Views...>>;
    return end_impl<true>(this, common_range<last_view_type>{});
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<
      std::is_void<Dummy>,
      sized_range<Views>...
  >::value, int> = 0>
  constexpr auto size() {
#if PREVIEW_CXX_VERSION < 17
    using CT = make_unsigned_like_t<common_type_t<decltype(ranges::size(std::declval<Views&>()))...>>;
    return preview::tuple_fold_left(
        preview::tuple_transform(views_, ranges::size),
        CT{0},
        std::plus<>{}
    );
#else
    return preview::apply(
        [](auto... sizes) {
          using CT = make_unsigned_like_t<common_type_t<decltype(sizes)...>>;
          return (CT(sizes) + ...);
        },
        preview::tuple_transform(views_, ranges::size)
    );
#endif
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<
      std::is_void<Dummy>,
      sized_range<const Views>...
  >::value, int> = 0>
  constexpr auto size() const {
#if PREVIEW_CXX_VERSION < 17
    using CT = make_unsigned_like_t<common_type_t<decltype(ranges::size(std::declval<const Views&>()))...>>;
    return preview::tuple_fold_left(
        preview::tuple_transform(views_, ranges::size),
        CT{0},
        std::plus<>{}
    );
#else
    return preview::apply(
        [](auto... sizes) {
          using CT = make_unsigned_like_t<common_type_t<decltype(sizes)...>>;
          return (CT(sizes) + ...);
        },
        preview::tuple_transform(views_, ranges::size)
    );
#endif
  }

 private:
  template<bool Const, typename This>
  constexpr static auto end_impl(This thiz, std::true_type /* common_range<maybe-const<is-const, Views...[N - 1]>> */) {
    return iterator<Const>(thiz, in_place_index<cranges - 1>,
                           ranges::end(std::get<cranges - 1>(thiz->views_)));
  }
  template<bool Const, typename This>
  constexpr static auto end_impl(This, std::false_type /* common_range<maybe-const<is-const, Views...[N - 1]>> */) {
    return default_sentinel;
  }
};

#if __cplusplus >= 201703L

template<typename... Rng>
concat_view(Rng &&...)
    -> concat_view<views::all_t<Rng>...>;

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
