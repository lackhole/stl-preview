//
// Created by yonggyulee on 2024/01/27.
//

#ifndef PREVIEW_RANGES_VIEWS_CARTESIAN_PRODUCT_VIEW_H_
#define PREVIEW_RANGES_VIEWS_CARTESIAN_PRODUCT_VIEW_H_

#include <limits>
#include <memory>
#include <tuple>
#include <utility>

#include "preview/__concepts/equality_comparable.h"
#include "preview/__iterator/default_sentinel_t.h"
#include "preview/__iterator/detail/have_cxx20_iterator.h"
#include "preview/__iterator/indirectly_swappable.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/iter_move.h"
#include "preview/__iterator/iter_swap.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__memory/addressof.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/bidirectional_range.h"
#include "preview/__ranges/common_range.h"
#include "preview/__ranges/simple_view.h"
#include "preview/__ranges/distance.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/random_access_range.h"
#include "preview/__ranges/range_value_t.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__tuple/tuple_transform.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/maybe_const.h"
#include "preview/__type_traits/negation.h"

namespace preview {
namespace ranges {
namespace detail {

template<bool Const, typename First, typename... Vs>
struct cartesian_product_is_random_access
    : conjunction<
          random_access_range<maybe_const<Const, First>>,
          random_access_range<maybe_const<Const, Vs>>...,
          sized_range<maybe_const<Const, Vs>>...
      > {};

template<typename R>
struct cartesian_product_common_arg
    : disjunction<
          common_range<R>,
          conjunction< sized_range<R>, random_access_range<R> >
      > {};

template<bool Const, typename First, typename... Vs>
struct cartesian_product_is_bidirectional
    : conjunction<
          bidirectional_range<maybe_const<Const, First>>,
          bidirectional_range<maybe_const<Const, Vs>>...,
          cartesian_product_common_arg<maybe_const<Const, Vs>>...
      > {};

template<typename First, typename... Vs>
struct cartesian_product_is_common : cartesian_product_common_arg<First> {};

template<typename... Vs>
struct cartesian_product_is_sized : conjunction<sized_range<Vs>...> {};

template< bool Const, template<typename> class FirstSent, typename First, typename... Vs>
struct cartesian_is_sized_sentinel
    : conjunction<
          sized_sentinel_for<
              FirstSent<maybe_const<Const, First>>,
              iterator_t<maybe_const<Const, First>>>,
          sized_range<maybe_const<Const, Vs>>...,
          sized_sentinel_for<
              iterator_t<maybe_const<Const, Vs>>,
              iterator_t<maybe_const<Const, Vs>>>...
      > {};

template<typename R>
constexpr auto cartesian_common_arg_end_impl(R& r, std::true_type /* common_range */) {
  return ranges::end(r);
}
template<typename R>
constexpr auto cartesian_common_arg_end_impl(R& r, std::false_type /* common_range */) {
  return ranges::begin(r) + ranges::distance(r);
}

template<typename R, std::enable_if_t<cartesian_product_common_arg<R>::value, int> = 0>
constexpr auto cartesian_common_arg_end(R& r) {
  return preview::ranges::detail::cartesian_common_arg_end_impl(r, common_range<R>{});
}

template<typename R, typename F, typename Tuple, std::size_t... I>
constexpr R cartesian_tuple_transform_impl(F func, Tuple&& t, std::index_sequence<I...>)
    noexcept(
        conjunction<bool_constant<
            noexcept(func(std::get<I>( std::forward<Tuple>(t) )))
        >...>::value
    )
{
  return R(func(std::get<I>( std::forward<Tuple>(t) ))...);
}

template<typename R, typename F, typename Tuple>
constexpr R cartesian_tuple_transform(F func, Tuple&& t)
    noexcept(noexcept(
        cartesian_tuple_transform_impl<R>(
            func,
            std::forward<Tuple>(t),
            std::make_index_sequence<std::tuple_size<remove_cvref_t<Tuple>>::value>{})
    ))
{
  return cartesian_tuple_transform_impl<R>(
      func,
      std::forward<Tuple>(t),
      std::make_index_sequence<std::tuple_size<remove_cvref_t<Tuple>>::value>{});
}

template<typename F, typename Tuple, std::size_t... I>
constexpr auto cartesian_tuple_transform_impl(F func, Tuple&& t, std::index_sequence<I...>) {
  return std::make_tuple(
      func(
          std::get<I>( std::forward<Tuple>(t) )
      )...
  );
}

template<typename F, typename Tuple>
constexpr auto cartesian_tuple_transform(F func, Tuple&& t) {
  return cartesian_tuple_transform_impl(
      func,
      std::forward<Tuple>(t),
      std::make_index_sequence<std::tuple_size<remove_cvref_t<Tuple>>::value>{});
}

template<typename Tuple, typename Indices>
struct cartesian_product_is_empty_impl;

template<typename Tuple, std::size_t... I>
struct cartesian_product_is_empty_impl<Tuple, std::index_sequence<0, I...>>
    : disjunction<

      > {};

template<typename Tuple, std::size_t N = std::tuple_size<Tuple>::value>
struct cartesian_product_is_empty : cartesian_product_is_empty_impl<Tuple, std::make_index_sequence<N>> {};

template<typename Tuple> struct cartesian_product_is_empty<Tuple, 1> : std::false_type {};
template<typename Tuple> struct cartesian_product_is_empty<Tuple, 0>; // unreachable

template<typename F, typename Tuple, std::size_t... I>
constexpr auto cartesian_tuple_transform_end(F func, const Tuple& t, std::index_sequence<0, I...>) {
  return std::make_tuple(func(std::get<I>(t))...);
}

} // namespace detail

template<typename First, typename... Vs>
class cartesian_product_view : public view_interface<cartesian_product_view<First, Vs...>> {
  std::tuple<First, Vs...> bases_;
  template<bool Const>
  using iterator_current = std::tuple<
        iterator_t<maybe_const<Const, First>>,
        iterator_t<maybe_const<Const, Vs   >>... >;

 public:
  static_assert(input_range<First>::value, "Constraints not satisfied");
  static_assert(conjunction<forward_range<Vs>...>::value, "Constraints not satisfied");
  static_assert(view<First>::value, "Constraints not satisfied");
  static_assert(conjunction<view<Vs>...>::value, "Constraints not satisfied");

  template<bool Const> class iterator;
  template<bool Const> friend class iterator;

  template<bool Const>
  class iterator {
    using Parent = maybe_const<Const, cartesian_product_view>;
    friend class cartesian_product_view;

   public:
    using iterator_category = input_iterator_tag;
    using iterator_concept =
        conditional_t<
            detail::cartesian_product_is_random_access<Const, First, Vs...>, random_access_iterator_tag,
            detail::cartesian_product_is_bidirectional<Const, First, Vs...>, bidirectional_iterator_tag,
            forward_range<maybe_const<Const, First>>, forward_iterator_tag,
            input_iterator_tag
        >;
    using value_type = std::tuple<
        range_value_t<maybe_const<Const, First>>,
        range_value_t<maybe_const<Const, Vs   >>... >;
    using reference = std::tuple<
        range_reference_t<maybe_const<Const, First>>,
        range_reference_t<maybe_const<Const, Vs   >>... >;
    using difference_type = int;
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
    using pointer = void;
#endif

    iterator() = default;

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<((Const != AntiConst) && Const)>,
        convertible_to<iterator_t<First>, iterator_t<const First>>,
        convertible_to<iterator_t<Vs>, iterator_t<const Vs>>...
    >::value, int> = 0>
    constexpr iterator(iterator<AntiConst> i)
        : parent_(i.parent_), current_(std::move(i.current_)) {}

    constexpr auto operator*() const {
      return detail::cartesian_tuple_transform(
          [](auto& i) -> decltype(auto) { return *i; }, current_);
    }

    template<typename First2 = First, std::enable_if_t<
        detail::cartesian_product_is_random_access<Const, First2, Vs...>::value, int> = 0>
    constexpr reference operator[](difference_type n) const {
      return *((*this) + n);
    }

    template<std::size_t N = sizeof...(Vs)>
    constexpr void next() {
      auto& it = std::get<N>(current_);
      ++it;
      next(it, std::integral_constant<std::size_t, N>{});
    }

    template<std::size_t N = sizeof...(Vs)>
    constexpr void prev() {
      auto& it = std::get<N>(current_);
      prev(it, std::integral_constant<std::size_t, N>{});
      --it;
    }

    template<typename Tuple>
    constexpr difference_type distance_from(const Tuple& t) const {
      return scaled_sum(t);
    }

    constexpr iterator& operator++() {
      next();
      return *this;
    }

    template<typename First2 = First, std::enable_if_t<
        forward_range<maybe_const<Const, First2>>::value == false, int> = 0>
    constexpr void operator++(int) {
      ++*this;
    }

    template<typename First2 = First, std::enable_if_t<
        forward_range<maybe_const<Const, First2>>::value, int> = 0>
    constexpr iterator operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    template<typename First2 = First, std::enable_if_t<
        detail::cartesian_product_is_bidirectional<Const, First2, Vs...>::value, int> = 0>
    constexpr iterator& operator--() {
      prev();
      return *this;
    }

    template<typename First2 = First, std::enable_if_t<
        detail::cartesian_product_is_bidirectional<Const, First2, Vs...>::value, int> = 0>
    constexpr iterator operator--(int) {
      auto tmp = *this;
      --*this;
      return tmp;
    }

    template<typename First2 = First, std::enable_if_t<
        detail::cartesian_product_is_random_access<Const, First2, Vs...>::value, int> = 0>
    constexpr iterator& operator+=(difference_type n) {
      if (n > 0) {
        while (n) {
          next();
          --n;
        }
      } else if (n < 0) {
        while (n < 0) {
          prev();
          ++n;
        }
      }
      return *this;
    }

    template<typename First2 = First, std::enable_if_t<
        detail::cartesian_product_is_random_access<Const, First2, Vs...>::value, int> = 0>
    constexpr iterator& operator-=(difference_type n) {
      *this += -n;
      return *this;
    }

    template<typename First2 = First, std::enable_if_t<
        equality_comparable<iterator_t<maybe_const<Const, First2>>>::value, int> = 0>
    friend constexpr bool operator==(const iterator& x, const iterator& y) {
      return x.current_ == y.current_;
    }

    template<typename First2 = First, std::enable_if_t<
        equality_comparable<iterator_t<maybe_const<Const, First2>>>::value, int> = 0>
    friend constexpr bool operator!=(const iterator& x, const iterator& y) {
      return !(x == y);
    }

    template<typename First2 = First, std::enable_if_t<
        equality_comparable<iterator_t<maybe_const<Const, First2>>>::value, int> = 0>
    friend constexpr bool operator==(const iterator& x, default_sentinel_t) {
      return x.compare_with_default();
    }

    template<typename First2 = First, std::enable_if_t<
        equality_comparable<iterator_t<maybe_const<Const, First2>>>::value, int> = 0>
    friend constexpr bool operator!=(const iterator& x, default_sentinel_t) {
      return !x.compare_with_default();
    }

    template<typename First2 = First, std::enable_if_t<
        equality_comparable<iterator_t<maybe_const<Const, First2>>>::value, int> = 0>
    friend constexpr bool operator==(default_sentinel_t, const iterator& x) {
      return x.compare_with_default();
    }

    template<typename First2 = First, std::enable_if_t<
        equality_comparable<iterator_t<maybe_const<Const, First2>>>::value, int> = 0>
    friend constexpr bool operator!=(default_sentinel_t, const iterator& x) {
      return !x.compare_with_default();
    }

    template<typename First2 = First, std::enable_if_t<
        detail::cartesian_product_is_random_access<Const, First2, Vs...>::value, int> = 0>
    friend constexpr iterator operator+(const iterator& i, difference_type n) {
      return iterator(i) += n;
    }

    template<typename First2 = First, std::enable_if_t<
        detail::cartesian_product_is_random_access<Const, First2, Vs...>::value, int> = 0>
    friend constexpr iterator operator+(difference_type n, const iterator& i) {
      return i + n;
    }

    template<typename First2 = First, std::enable_if_t<
        detail::cartesian_product_is_random_access<Const, First2, Vs...>::value, int> = 0>
    friend constexpr iterator operator-(const iterator& i, difference_type n) {
      return iterator(i) -= n;
    }

    template<typename First2 = First, std::enable_if_t<
        detail::cartesian_is_sized_sentinel<Const, iterator_t, First2, Vs...>::value, int> = 0>
    friend constexpr difference_type operator-(const iterator& i, const iterator& j) {
      return i.distance_from(j.current_);
    }

    template<typename First2 = First, std::enable_if_t<
        detail::cartesian_is_sized_sentinel<Const, sentinel_t, First2, Vs...>::value, int> = 0>
    friend constexpr difference_type operator-(const iterator& i, default_sentinel_t) {
      return i.distance_from_default();
    }

    template<typename First2 = First, std::enable_if_t<
        detail::cartesian_is_sized_sentinel<Const, sentinel_t, First2, Vs...>::value, int> = 0>
    friend constexpr difference_type operator-(default_sentinel_t s, const iterator& i) {
      return -(i - s);
    }

     friend constexpr auto iter_move(const iterator& i)
         noexcept(
             noexcept(preview::tuple_transform(i.current_, ranges::iter_move)) &&
             conjunction<
                 std::is_nothrow_move_constructible<range_rvalue_reference_t< maybe_const<Const, First> >>,
                 std::is_nothrow_move_constructible<range_rvalue_reference_t< maybe_const<Const, Vs>    >>...
             >::value
         )
     {
       return preview::tuple_transform(i.current_, ranges::iter_move);
     }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
        indirectly_swappable< iterator_t<maybe_const<Const, First>> >,
        indirectly_swappable< iterator_t<maybe_const<Const, Vs>> >...
    >::value, int> = 0>
    friend constexpr void iter_swap(const iterator& x, const iterator& y)
        noexcept(noexcept(x.iter_swap_impl(y)))
    {
      x.iter_swap_impl(y);
    }

   private:
    template<typename It>
    constexpr void next(It&, std::integral_constant<std::size_t, 0>) {}
    template<typename It, std::size_t N>
    constexpr void next(It& it, std::integral_constant<std::size_t, N>) {
      if (it == ranges::end(std::get<N>(parent_->bases_))) {
        it = ranges::begin(std::get<N>(parent_->bases_));
        next<N - 1>();
      }
    }

    template<typename It>
    constexpr void prev(It&, std::integral_constant<std::size_t, 0>) {}
    template<typename It, std::size_t N>
    constexpr void prev(It& it, std::integral_constant<std::size_t, N>) {
      if (it == ranges::begin(std::get<N>(parent_->bases_))) {
        it = detail::cartesian_common_arg_end(std::get<N>(parent_->bases_));
        prev<N - 1>();
      }
    }

    template<typename Tuple>
    constexpr difference_type scaled_size(const Tuple& t, std::integral_constant<std::size_t, 0>) const {
      // f = 0
      return static_cast<difference_type>(std::get<0>(current_) - std::get<0>(t));
    }
    template<typename Tuple, std::size_t N>
    constexpr difference_type scaled_size(const Tuple& t, std::integral_constant<std::size_t, N>) const {
      auto f = static_cast<difference_type>(ranges::size(std::get<N>(parent_->bases_)));
      auto g = static_cast<difference_type>(std::get<N>(current_) - std::get<N>(t));
      return g + f * scaled_size(t, std::integral_constant<std::size_t, N - 1>{});
    }

    template<typename Tuple>
    constexpr difference_type scaled_sum(const Tuple& t) const {
      return scaled_size(t, std::integral_constant<std::size_t, sizeof...(Vs)>{});
    }

    template<std::size_t... I>
    constexpr difference_type distance_from_default_impl(std::index_sequence<0, I...>) const {
      return distance_from(std::make_tuple(
          ranges::end(std::get<0>(parent_->bases_)),
          ranges::begin(std::get<I>(parent_->bases_))...));
    }

    constexpr difference_type distance_from_default() const {
      return distance_from_default_impl(std::index_sequence_for<Vs...>{});
    }

    constexpr bool compare_with_default_impl(std::integral_constant<std::size_t, 0>) const {
      return std::get<0>(current_) == ranges::end(std::get<0>(parent_->bases_));
    }

    template<std::size_t I>
    constexpr bool compare_with_default_impl(std::integral_constant<std::size_t, I>) const {
      return std::get<I>(current_) == ranges::end(std::get<I>(parent_->bases_))
          ? true
          : compare_with_default(std::integral_constant<std::size_t, I - 1>{});
    }

    constexpr bool compare_with_default() const {
      return compare_with_default_impl(std::index_sequence_for<Vs...>{});
    }

    template<std::size_t... I>
    constexpr void iter_swap_impl(const iterator& other, std::index_sequence<I...>) const
        noexcept(
            conjunction<bool_constant<
                noexcept(ranges::iter_swap(std::get<I>(this->current_), std::get<I>(other.current_)))
            >...>::value
        )
    {
      int dummy[] = {
          (ranges::iter_swap(std::get<I>(current_), std::get<I>(other.current_)), 0)...
      };
      (void)dummy;
    }

    constexpr iterator(Parent& parent, iterator_current<Const> current)
        : parent_(preview::addressof(parent)), current_(std::move(current)) {}

    Parent* parent_ = nullptr;
    iterator_current<Const> current_;
  };

  constexpr cartesian_product_view() = default;

  constexpr cartesian_product_view(First first, Vs... bases)
      : bases_(std::move(first), std::move(bases)...) {}

  template<typename First2 = First, std::enable_if_t<disjunction<
      negation< simple_view<First2> >,
      negation< simple_view<Vs> >...
  >::value, int> = 0>
  constexpr iterator<false> begin() {
    return iterator<false>(
        *this, detail::cartesian_tuple_transform<iterator_current<true>>(ranges::begin, bases_));
  }

  template<typename First2 = First, std::enable_if_t<conjunction<
      range<const First2>,
      range<const Vs>...
  >::value, int> = 0>
  constexpr iterator<true> begin() const {
    return iterator<true>(
        *this, detail::cartesian_tuple_transform<iterator_current<true>>(ranges::begin, bases_));
  }

  template<typename First2 = First, std::enable_if_t<conjunction<
      disjunction<
          negation< simple_view<First2> >,
          negation< simple_view<Vs> >...>,
      detail::cartesian_product_is_common<First, Vs...>
  >::value, int> = 0>
  constexpr iterator<false> end() {
    return iterator<false>(*this, tuple_transform_end<false>(bases_));
  }

  template<typename First2 = First, std::enable_if_t<
      detail::cartesian_product_is_common<const First2, const Vs...>::value, int> = 0>
  constexpr iterator<true> end() const {
    return iterator<true>(*this, tuple_transform_end<true>(bases_));
  }

  template<typename First2 = First, std::enable_if_t<
      detail::cartesian_product_is_common<const First2, const Vs...>::value == false, int> = 0>
  constexpr default_sentinel_t end() const {
    return default_sentinel;
  }

  template<typename First2 = First, std::enable_if_t<
      detail::cartesian_product_is_sized<First2, Vs...>::value, int> = 0>
  constexpr std::size_t size() {
    return size_impl(std::index_sequence_for<First, Vs...>{});
  }

  template<typename First2 = First, std::enable_if_t<
      detail::cartesian_product_is_sized<const First2, const Vs...>::value, int> = 0>
  constexpr std::size_t size() const {
    return size_impl(std::index_sequence_for<First, Vs...>{});
  }

 private:
  template<std::size_t N>
  constexpr bool is_empty_impl(std::integral_constant<std::size_t, N>) const {
    return ranges::empty(std::get<N>(bases_)) ? true : is_empty_impl(std::integral_constant<std::size_t, N - 1>{});
  }
  constexpr bool is_empty_impl(std::integral_constant<std::size_t, 0>) const {
    return false;
  }
  constexpr bool is_empty() const {
    return is_empty_impl(std::integral_constant<std::size_t, sizeof...(Vs)>{});
  }

  template<bool Const, typename Tuple, std::size_t... I>
  constexpr iterator_current<Const> tuple_transform_end_impl(Tuple&& t, std::index_sequence<0, I...>) {
    return iterator_current<Const>(
        is_empty() ?
            ranges::begin( std::get<0>(std::forward<Tuple>(t)) ) :
            detail::cartesian_common_arg_end( std::get<0>(std::forward<Tuple>(t)) ),
        ranges::begin(std::get<I>(std::forward<Tuple>(t)))...
    );
  }

  template<bool Const, typename Tuple>
  constexpr iterator_current<Const> tuple_transform_end(Tuple&& t) {
    return tuple_transform_end_impl<Const>(
        std::forward<Tuple>(t),
        std::make_index_sequence<std::tuple_size<remove_cvref_t<Tuple>>::value>{});
  }

  template<bool Const, typename Tuple, std::size_t... I>
  constexpr iterator_current<Const> tuple_transform_end_impl(Tuple&& t, std::index_sequence<0, I...>) const {
    return iterator_current<Const>(
        is_empty() ?
            ranges::begin( std::get<0>(std::forward<Tuple>(t)) ) :
            detail::cartesian_common_arg_end( std::get<0>(std::forward<Tuple>(t)) ),
        ranges::begin(std::get<I>(std::forward<Tuple>(t)))...
    );
  }

  template<bool Const, typename Tuple>
  constexpr iterator_current<Const> tuple_transform_end(Tuple&& t) const {
    return tuple_transform_end_impl<Const>(
        std::forward<Tuple>(t),
        std::make_index_sequence<std::tuple_size<remove_cvref_t<Tuple>>::value>{});
  }

  template<std::size_t... I>
  constexpr std::size_t size_impl(std::index_sequence<I...>) {
    std::size_t prod = 1;
    int dummy[] = {
        ((void)(prod *= static_cast<std::size_t>(ranges::size(std::get<I>(bases_)))), 0)...
    };
    (void)dummy;
    return prod;
  }

  template<std::size_t... I>
  constexpr std::size_t size_impl(std::index_sequence<I...>) const {
    std::size_t prod = 1;
    int dummy[] = {
        ((void)(prod *= static_cast<std::size_t>(ranges::size(std::get<I>(bases_)))), 0)...
    };
    (void)dummy;
    return prod;
  }
};

#if __cplusplus >= 201703L

template<typename... Rs>
cartesian_product_view(Rs&&...) -> cartesian_product_view<views::all_t<Rs>...>;

#endif

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_CARTESIAN_PRODUCT_VIEW_H_
