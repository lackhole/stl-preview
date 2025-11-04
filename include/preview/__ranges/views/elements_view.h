//
// Created by yonggyulee on 2024/01/26.
//

#ifndef PREVIEW_RANGES_VIEWS_ELEMENTS_VIEW_H_
#define PREVIEW_RANGES_VIEWS_ELEMENTS_VIEW_H_

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "preview/__concepts/copy_constructible.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__concepts/default_initializable.h"
#include "preview/__concepts/derived_from.h"
#include "preview/__concepts/move_constructible.h"
#include "preview/__iterator/detail/have_cxx20_iterator.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__ranges/approximately_sized_range.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/bidirectional_range.h"
#include "preview/__ranges/common_range.h"
#include "preview/__ranges/simple_view.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/random_access_range.h"
#include "preview/__ranges/range_reference_t.h"
#include "preview/__ranges/range_value_t.h"
#include "preview/__ranges/reserve_hint.h"
#include "preview/__ranges/size.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__ranges/viewable_range.h"
#include "preview/__tuple/tuple_like.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/maybe_const.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename T, std::size_t N, bool = tuple_like<T>::value /* false */>
struct has_tuple_element : std::false_type {};
template<typename T, std::size_t N>
struct has_tuple_element<T, N, true> : bool_constant<(N < std::tuple_size<T>::value)> {};

template<typename T, std::size_t N,
    bool = std::is_reference<T>::value,
    bool = has_typename_type<std::tuple_element<N, T>>::value>
struct returnable_element : std::false_type {};
template<typename T, std::size_t N, bool v>
struct returnable_element<T, N, true, v> : std::true_type {};
template<typename T, std::size_t N>
struct returnable_element<T, N, false, true>
    : move_constructible<std::tuple_element_t<N, T>> {};

// Not defined, if Base does not model forward_range
template<typename Base, std::size_t N, bool = forward_range<Base>::value /* false */>
struct elements_view_iterator_category {
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
  using iterator_category = iterator_ignore;
#endif
};

template<typename Base, std::size_t N>
struct elements_view_iterator_category<Base, N, true> {
 private:
  using C = typename iterator_traits<iterator_t<Base>>::iterator_category;

 public:
  using iterator_category =
      conditional_t<
          std::is_rvalue_reference<decltype(std::get<N>(*std::declval<iterator_t<Base>&>()))>, input_iterator_tag,
          derived_from<C, random_access_iterator_tag>, random_access_iterator_tag,
          C
      >;
};

} // namespace detail

template<typename V, std::size_t N>
class elements_view : public view_interface<elements_view<V, N>> {
 public:
  static_assert(input_range<V>::value, "Constraints not satisfied");
  static_assert(view<V>::value, "Constraints not satisfied");
  static_assert(detail::has_tuple_element<range_value_t<V>, N>::value, "Constraints not satisfied");
  static_assert(detail::has_tuple_element<std::remove_reference_t<range_reference_t<V>>, N>::value, "Constraints not satisfied");
  static_assert(detail::returnable_element<range_reference_t<V>, N>::value, "Constraints not satisfied");

  template<bool> class iterator;
  template<bool> class sentinel;

  template<bool Const>
  class iterator : public detail::elements_view_iterator_category<std::conditional_t<Const, const V, V>, N> {
    using Base = maybe_const<Const, V>;
    template<bool> friend class sentinel;

    template<typename Ref, bool = std::is_reference<Ref>::value /* true */>
    struct iterator_reference {
      using type = decltype(std::get<N>(*std::declval<iterator_t<Base>&>()));
    };
    template<typename Ref>
    struct iterator_reference<Ref, false> {
      using type = std::remove_cv_t<std::tuple_element_t<N, Ref>>;
    };

   public:
    using iterator_concept =
        conditional_t<
            random_access_range<Base>, random_access_iterator_tag,
            bidirectional_range<Base>, bidirectional_iterator_tag,
            forward_range<Base>, forward_iterator_tag,
            input_iterator_tag
        >;
    using value_type = remove_cvref_t<std::tuple_element_t<N, range_value_t<Base>>>;
    using difference_type = range_difference_t<Base>;
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
    using pointer = void;
    using reference = typename iterator_reference<range_reference_t<Base>>::type;
#endif

    template<bool B = default_initializable<iterator_t<Base>>::value, std::enable_if_t<B, int> = 0>
    constexpr iterator() {}

    constexpr explicit iterator(iterator_t<Base> current)
        : current_(std::move(current)) {}

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<((Const != AntiConst) && Const)>,
        convertible_to<iterator_t<V>, iterator_t<Base>>
    >::value, int> = 0>
    constexpr iterator(iterator<AntiConst> i)
        : current_(std::move(i.current_)) {}

    constexpr const iterator_t<Base>& base() const& noexcept {
      return current_;
    }

    constexpr iterator_t<Base> base() && {
      return std::move(current_);
    }

    constexpr decltype(auto) operator*() const {
      return get_element(this->base(), std::is_reference<range_reference_t<Base>>{});
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    constexpr decltype(auto) operator[](difference_type n) const {
      return get_element(this->base() + n, std::is_reference<range_reference_t<Base>>{});
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
      return [&](){ auto j = i; j += n; return j; }();
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    friend constexpr iterator operator+(difference_type n, iterator i) {
      return [&](){ auto j = i; j += n; return j; }();
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    friend constexpr iterator operator-(iterator i, difference_type n) {
      return [&](){ auto j = i; j -= n; return j; }();
    }

    template<typename B = Base, std::enable_if_t<sized_sentinel_for<iterator_t<B>, iterator_t<B>>::value, int> = 0>
    friend constexpr difference_type operator-(const iterator& x, const iterator& y) {
      return x.current_ - y.current_;
    }

   private:
    template<typename T>
    constexpr decltype(auto) get_element(T&& e, std::true_type /* reference */) const {
      return std::get<N>(*std::forward<T>(e));
    }

    template<typename T>
    constexpr decltype(auto) get_element(T&& e, std::false_type /* reference */) const {
      using E = std::remove_cv_t<std::tuple_element_t<N, range_reference_t<Base>>>;
      return static_cast<E>(std::get<N>(*std::forward<T>(e)));
    }

    iterator_t<Base> current_{};
  };

  template<bool Const>
  class sentinel {
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

    template<bool OtherConst, std::enable_if_t<
        sentinel_for<
            sentinel_t<Base>,
            iterator_t<maybe_const<OtherConst, V>>
        >::value, int> = 0>
    friend constexpr bool operator==(const iterator<OtherConst>& x, const sentinel& y) {
      using namespace preview::rel_ops;
      return x.base() == y.end_;
    }

    template<bool OtherConst, std::enable_if_t<
        sentinel_for<
            sentinel_t<Base>,
            iterator_t<maybe_const<OtherConst, V>>
        >::value, int> = 0>
    friend constexpr bool operator!=(const iterator<OtherConst>& x, const sentinel& y) {
      return !(x == y);
    }

    template<bool OtherConst, std::enable_if_t<
        sentinel_for<
            sentinel_t<Base>,
            iterator_t<maybe_const<OtherConst, V>>
        >::value, int> = 0>
    friend constexpr bool operator==(const sentinel& y, const iterator<OtherConst>& x) {
      return x == y;
    }

    template<bool OtherConst, std::enable_if_t<
        sentinel_for<
            sentinel_t<Base>,
            iterator_t<maybe_const<OtherConst, V>>
        >::value, int> = 0>
    friend constexpr bool operator!=(const sentinel& y, const iterator<OtherConst>& x) {
      return !(x == y);
    }

    template<bool OtherConst, std::enable_if_t<
        sized_sentinel_for<
            sentinel_t<Base>,
            iterator_t<maybe_const<OtherConst, V>>
        >::value, int> = 0>
    friend constexpr range_difference_t<maybe_const<OtherConst, V>>
    operator-(const iterator<OtherConst>& x, const sentinel& y) {
      return x.base() - y.end_;
    }

    template<bool OtherConst, std::enable_if_t<
        sized_sentinel_for<
            sentinel_t<Base>,
            iterator_t<maybe_const<OtherConst, V>>
        >::value, int> = 0>
    friend constexpr range_difference_t<maybe_const<OtherConst, V>>
    operator-(const sentinel& y, const iterator<OtherConst>& x) {
      return y.end_ - x.base();
    }


   private:
    sentinel_t<Base> end_;
  };

  template<bool B = default_initializable<V>::value, std::enable_if_t<B, int> = 0>
  constexpr elements_view() {}

  constexpr explicit elements_view(V base)
      : base_(std::move(base)) {}

  template<typename V2 = V, std::enable_if_t<copy_constructible<V2>::value, int> = 0>
  constexpr V base() const& {
    return base_;
  }

  constexpr V base() && {
    return std::move(base_);
  }

  template<typename V2 = V, std::enable_if_t<simple_view<V2>::value == false, int> = 0>
  constexpr auto begin() {
    return iterator<false>{ranges::begin(base_)};
  }

  template<typename V2 = V, std::enable_if_t<range<const V2>::value, int> = 0>
  constexpr auto begin() const {
    return iterator<true>{ranges::begin(base_)};
  }

  template<typename V2 = V, std::enable_if_t<conjunction<
      negation< simple_view<V2> >,
      negation< common_range<V2> >
  >::value, int> = 0>
  constexpr auto end() {
    return sentinel<false>{ranges::end(base_)};
  }

  template<typename V2 = V, std::enable_if_t<conjunction<
      negation< simple_view<V2> >,
      common_range<V2>
  >::value, int> = 0>
  constexpr auto end() {
    return iterator<false>{ranges::end(base_)};
  }

  template<typename V2 = V, std::enable_if_t<conjunction<
      range<const V2>,
      negation< common_range<const V2> >
  >::value, int> = 0>
  constexpr auto end() const {
    return sentinel<true>{ranges::end(base_)};
  }

  template<typename V2 = V, std::enable_if_t<conjunction<
      common_range<const V2>
  >::value, int> = 0>
  constexpr auto end() const {
    return iterator<true>{ranges::end(base_)};
  }

  template<typename V2 = V, std::enable_if_t<sized_range<V2>::value, int> = 0>
  constexpr auto size() {
    return ranges::size(base_);
  }

  template<typename V2 = V, std::enable_if_t<sized_range<const V2>::value, int> = 0>
  constexpr auto size() const {
    return ranges::size(base_);
  }

  template<bool B = approximately_sized_range<V>::value, std::enable_if_t<B, int> = 0>
  constexpr auto reserve_hint() {
    return ranges::reserve_hint(base_);
  }
  template<bool B = approximately_sized_range<const V>::value, std::enable_if_t<B, int> = 0>
  constexpr auto reserve_hint() const {
    return ranges::reserve_hint(base_);
  }

 private:
  V base_{};
};

} // namespace ranges
} // namespace preview

template<typename V, std::size_t N>
PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(preview::ranges::elements_view<V, N>)
    = preview::ranges::enable_borrowed_range<V>;

#endif // PREVIEW_RANGES_VIEWS_ELEMENTS_VIEW_H_
