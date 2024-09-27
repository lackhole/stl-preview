//
// Created by yonggyulee on 2024/01/04.
//

#ifndef PREVIEW_RANGES_SUBRANGE_H_
#define PREVIEW_RANGES_SUBRANGE_H_

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "preview/__core/nodiscard.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__concepts/copyable.h"
#include "preview/__concepts/different_from.h"
#include "preview/__concepts/same_as.h"
#include "preview/__iterator/advance.h"
#include "preview/__iterator/bidirectional_iterator.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/input_or_output_iterator.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/borrowed_range.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/sentinel_t.h"
#include "preview/__ranges/size.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__ranges/subrange_kind.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__tuple/specialize_tuple.h"
#include "preview/__tuple/tuple_like.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/make_unsigned_like.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__utility/cxx20_rel_ops.h"
#include "preview/__utility/in_place.h"
#include "preview/__utility/to_unsigned_like.h"

namespace preview {
namespace ranges {
namespace detail {

template<class From, class To>
struct uses_nonqualification_pointer_conversion
    : conjunction<
        std::is_pointer<From>,
        std::is_pointer<To>,
        negation<convertible_to<std::remove_pointer_t<From>(*)[], std::remove_pointer_t<To>(*)[]>>
    > {};

template<typename From, typename To>
struct convertible_to_non_slicing
    : conjunction<
          convertible_to<From, To>,
          negation<uses_nonqualification_pointer_conversion<From, To>>
      > {};

template<typename T, typename U, typename V, bool = pair_like<T>::value /* false */>
struct pair_like_convertible_from : std::false_type {};

template<typename T, typename U, typename V>
struct pair_like_convertible_from<T, U, V, true>
    : conjunction<
        negation< range<T> >,
        negation< std::is_reference<T> >,
        constructible_from<T, U, V>,
        convertible_to_non_slicing<U, std::tuple_element_t<0, T>>,
        convertible_to<V, std::tuple_element_t<1, T>>
    > {};

template<typename I, typename S, bool Store /* false */>
struct subrange_size {
  constexpr subrange_size() = default;
  template<typename U>
  constexpr subrange_size(in_place_t, U) noexcept {}
};

template<typename I, typename S>
struct subrange_size<I, S, true> {
  constexpr subrange_size() = default;
  template<typename U>
  constexpr subrange_size(in_place_t, U n) : size_(n) {}

  make_unsigned_like_t<iter_difference_t<I>> size_ = 0;
};

} // namespace detail

template<
    typename I,
    typename S = I,
    subrange_kind K = sized_sentinel_for<S, I>::value ? subrange_kind::sized : subrange_kind::unsized
>
class subrange
    : public view_interface<subrange<I, S, K>>
    , detail::subrange_size<I, S, (K == subrange_kind::sized && sized_sentinel_for<S, I>::value == false)>
{
  using StoreSize = bool_constant<(K == subrange_kind::sized && sized_sentinel_for<S, I>::value == false)>;
  using size_base = detail::subrange_size<I, S, StoreSize::value>;
 public:
  static_assert(input_or_output_iterator<I>::value, "Constraints not satisfied");
  static_assert(sentinel_for<S, I>::value, "Constraints not satisfied");
  static_assert(K == subrange_kind::sized || !sized_sentinel_for<S, I>::value, "Constraints not satisfied");

  subrange() = default;

  template<typename I2, std::enable_if_t<conjunction<
      detail::convertible_to_non_slicing<I2, I>,
      negation<StoreSize>
  >::value, int> = 0>
  constexpr subrange(I2 i, S s)
      : iterator_(std::move(i)), sentinel_(std::move(s)) {}

  template<typename I2, std::enable_if_t<conjunction<
      detail::convertible_to_non_slicing<I2, I>,
      bool_constant< K == subrange_kind::sized >
  >::value, int> = 0>
  constexpr subrange(I2 i, S s, make_unsigned_like_t<iter_difference_t<I>> n)
      : size_base(in_place, n), iterator_(std::move(i)), sentinel_(std::move(s)) {}

  template<typename R, std::enable_if_t<conjunction<
      different_from<subrange, R>,
      borrowed_range<R>,
      detail::convertible_to_non_slicing<iterator_t<R>, I>,
      convertible_to<sentinel_t<R>, S>,
      disjunction<
          negation<StoreSize>,
          sized_range<R>
      >
  >::value, int> = 0>
  constexpr subrange(R&& r)
      : subrange(r, static_cast<make_unsigned_like_t<iter_difference_t<I>>>(ranges::size(r))) {}

  template<typename R, std::enable_if_t<conjunction<
      borrowed_range<R>,
      detail::convertible_to_non_slicing<iterator_t<R>, I>,
      convertible_to<sentinel_t<R>, S>,
      bool_constant< K == subrange_kind::sized >
  >::value, int> = 0>
  constexpr subrange(R&& r, make_unsigned_like_t<iter_difference_t<I>> n)
      : subrange(ranges::begin(r), ranges::end(r), n) {}


  template<typename PairLike, std::enable_if_t<conjunction<
      negation< same_as<remove_cvref_t<PairLike>, subrange> >,
      detail::pair_like_convertible_from<PairLike, const I&, const S&>
  >::value, int> = 0>
  constexpr operator PairLike() const {
    return PairLike(iterator_, sentinel_);
  }


  template<typename I2 = I, std::enable_if_t<copyable<I2>::value, int> = 0>
  constexpr I begin() const {
    return iterator_;
  }

  template<typename I2 = I, std::enable_if_t<copyable<I2>::value == false, int> = 0>
  PREVIEW_NODISCARD constexpr I begin() {
    return std::move(iterator_);
  }

  constexpr S end() const {
    return sentinel_;
  }

  constexpr bool empty() const {
    using namespace preview::rel_ops;
    return iterator_ == sentinel_;
  }

  template<subrange_kind K2 = K, std::enable_if_t<conjunction<
      bool_constant<K2 == subrange_kind::sized>
  >::value, int> = 0>
  constexpr make_unsigned_like_t<iter_difference_t<I>> size() const {
    return size_impl(StoreSize{});
  }

  constexpr subrange& advance(iter_difference_t<I> n) {
    advance_impl(std::move(n), bidirectional_iterator<I>{}, StoreSize{});
    return *this;
  }

  template<typename I2 = I, std::enable_if_t<bidirectional_iterator<I2>::value, int> = 0>
  PREVIEW_NODISCARD constexpr subrange_kind prev(iter_difference_t<I> n = 1) const {
    auto tmp = *this;
    tmp.advance(-n);
    return tmp;
  }

  template<typename I2 = I, std::enable_if_t<forward_iterator<I2>::value, int> = 0>
  PREVIEW_NODISCARD constexpr subrange_kind next(iter_difference_t<I> n = 1) const& {
    auto tmp = *this;
    tmp.advance(n);
    return tmp;
  }

  template<typename I2 = I, std::enable_if_t<forward_iterator<I2>::value, int> = 0>
  PREVIEW_NODISCARD constexpr subrange_kind next(iter_difference_t<I> n = 1) && {
    advance(n);
    return std::move(*this);
  }

 private:
  constexpr auto size_impl(std::true_type) const {
    return size_base::size_;
  }

  constexpr auto size_impl(std::false_type) const {
    return preview::to_unsigned_like(sentinel_ - iterator_);
  }

  constexpr void advance_impl(iter_difference_t<I> n, std::true_type /* bidirectional_iterator<I>{} */, std::true_type /* StoreSize */) {
    if (n < 0) {
      ranges::advance(iterator_, n);
      size_base::size_ += preview::to_unsigned_like(-n);
    }
  }

  constexpr void advance_impl(iter_difference_t<I> n, std::true_type, std::false_type) {
    if (n < 0)
      ranges::advance(iterator_, n);
  }

  constexpr void advance_impl(iter_difference_t<I> n, std::false_type, std::true_type) {
    auto d = n - ranges::advance(iterator_, n, sentinel_);
    size_base::size_ -= preview::to_unsigned_like(d);
  }

  constexpr void advance_impl(iter_difference_t<I> n, std::false_type, std::false_type) {
    ranges::advance(iterator_, n, sentinel_);
  }

  I iterator_;
  S sentinel_;
};

// make-function for C++14

template<typename I, typename S, std::enable_if_t<conjunction<
    input_or_output_iterator<I>,
    sentinel_for<S, I>
>::value, int> = 0>
constexpr subrange<I, S>
make_subrange(I i, S s) {
  return subrange<I, S>(std::move(i), std::move(s));
}

template<typename I, typename S, std::enable_if_t<conjunction<
    input_or_output_iterator<I>,
    sentinel_for<S, I>
>::value, int> = 0>
constexpr subrange<I, S, subrange_kind::sized>
make_subrange(I i, S s, make_unsigned_like_t<iter_difference_t<I>> n) {
  return subrange<I, S, subrange_kind::sized>(std::move(i), std::move(s), n);
}

namespace detail {

template<typename R, bool = /* true */ sized_range<R>::value || sized_sentinel_for<sentinel_t<R>, iterator_t<R>>::value>
struct subrange_kind_judge {
  static constexpr subrange_kind value = subrange_kind::sized;
};
template<typename R>
struct subrange_kind_judge<R, false> {
  static constexpr subrange_kind value = subrange_kind::unsized;
};

} // namespace detail

template<typename R, std::enable_if_t<borrowed_range<R>::value, int> = 0>
constexpr subrange<iterator_t<R>, sentinel_t<R>, detail::subrange_kind_judge<R>::value> make_subrange(R&& r) {
  return {std::forward<R>(r)};
}

template<typename R, std::enable_if_t<borrowed_range<R>::value, int> = 0>
constexpr subrange<iterator_t<R>, sentinel_t<R>, subrange_kind::sized>
make_subrange(R&& r, make_unsigned_like_t<range_difference_t<R>> n) {
  return {std::forward<R>(r), n};
}


#if PREVIEW_CXX_VERSION >= 17
// These two are ambiguous without constraints
// vvvvvvvvvvvvvvvvvvvv
template<typename R>
subrange(R&&, make_unsigned_like_t<range_difference_t<R>>) ->
    subrange<
            std::enable_if_t<borrowed_range<R>::value,
        ranges::iterator_t<R>>,
        ranges::sentinel_t<R>,
        ranges::subrange_kind::sized
    >;

template<typename I, typename S>
subrange(I, S)
    -> subrange<
            std::enable_if_t<conjunction<
                input_or_output_iterator<I>,
                sentinel_for<S, I>
            >::value,
        I>,
        S
    >;
// ^^^^^^^^^^^^^^^^^^^^

template<typename I, typename S>
subrange(I, S, make_unsigned_like_t<iter_difference_t<I>>) ->
    subrange<I, S, ranges::subrange_kind::sized>;

template<typename R>
subrange(R&&) -> subrange<ranges::iterator_t<R>, ranges::sentinel_t<R>, detail::subrange_kind_judge<R>::value>;

#endif

namespace detail {

template<typename R>
struct is_size_storing_subrange;

template<typename I, typename S, subrange_kind K>
struct is_size_storing_subrange<subrange<I, S, K>>
    : bool_constant<(K == subrange_kind::sized && sized_sentinel_for<S, I>::value == false)> {};

template<std::size_t N>
struct get_subrange;

template<> struct get_subrange<0> {
  template<typename I, typename S, subrange_kind K>
  static constexpr auto get(const subrange<I, S, K>& r) {
    return r.begin();
  }
  template<typename I, typename S, subrange_kind K>
  static constexpr auto get(subrange<I, S, K>&& r) {
    return std::move(r.begin());
  }
};

template<> struct get_subrange<1> {
  template<typename I, typename S, subrange_kind K>
  auto get(const subrange<I, S, K>& r) {
    return r.end();
  }
  template<typename I, typename S, subrange_kind K>
  auto get(subrange<I, S, K>&& r) {
    return std::move(r.end());
  }
};

template<typename T>
struct is_subrange : std::false_type {};
template<typename I, typename S, subrange_kind K>
struct is_subrange<subrange<I, S, K>> : std::true_type {};

} // namespace detail

template<std::size_t N, typename I, typename S, subrange_kind K,
    std::enable_if_t<((N == 0 && copyable<I>::value) || N == 1), int> = 0>
constexpr auto get(const subrange<I, S, K>& r) {
  return detail::get_subrange<N>::get(r);
}

template<std::size_t N, typename I, typename S, subrange_kind K,
    std::enable_if_t<(N < 2), int> = 0>
constexpr auto get(subrange<I, S, K>&& r) {
  return detail::get_subrange<N>::get(std::move(r));
}

} // namespace ranges

namespace internal {

template<typename I, typename S, ranges::subrange_kind K>
struct tuple_like_uncvref<ranges::subrange<I, S, K>> : std::true_type {};

} // namespace internal

} // namespace preview

template<typename I, typename S, preview::ranges::subrange_kind K>
PREVIEW_SPECIALIZE_STD_TUPLE_SIZE(preview::ranges::subrange<I, S, K>)
    : public std::integral_constant<std::size_t, 2> {};

template<typename I, typename S, preview::ranges::subrange_kind K>
PREVIEW_SPECIALIZE_STD_TUPLE_ELEMENT(0, preview::ranges::subrange<I, S, K>) { using type = I; };
template<typename I, typename S, preview::ranges::subrange_kind K>
PREVIEW_SPECIALIZE_STD_TUPLE_ELEMENT(0, const preview::ranges::subrange<I, S, K>) { using type = I; };
template<typename I, typename S, preview::ranges::subrange_kind K>
PREVIEW_SPECIALIZE_STD_TUPLE_ELEMENT(1, preview::ranges::subrange<I, S, K>) { using type = S; };
template<typename I, typename S, preview::ranges::subrange_kind K>
PREVIEW_SPECIALIZE_STD_TUPLE_ELEMENT(1, const preview::ranges::subrange<I, S, K>) { using type = S; };

namespace std {

using ::preview::ranges::get;

} // namespace std

template<typename I, typename S, preview::ranges::subrange_kind K>
PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(preview::ranges::subrange<I, S, K>) = true;

#endif // PREVIEW_RANGES_SUBRANGE_H_
