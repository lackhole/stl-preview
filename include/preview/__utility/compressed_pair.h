# /*
#  * Created by YongGyu Lee on 2021/11/14.
#  */
#
# ifndef PREVIEW_UTILITY_COMPRESSED_PAIR_H_
# define PREVIEW_UTILITY_COMPRESSED_PAIR_H_
#
# include <cstddef>
# include <type_traits>
# include <tuple>
# include <utility>
#
# include "preview/__core/inline_variable.h"
# include "preview/__concepts/different_from.h"
# include "preview/__ranges/subrange.h"
# include "preview/__tuple/make_from_tuple.h"
# include "preview/__tuple/specialize_tuple.h"
# include "preview/__tuple/tuple_like.h"
# include "preview/__type_traits/conjunction.h"
# include "preview/__type_traits/is_swappable.h"
# include "preview/__type_traits/unwrap_reference.h"

namespace preview {
namespace detail {

template<typename T, bool = /* false */ std::is_empty<T>::value>
struct basic_compressed_slot {
  template<typename... Args, std::enable_if_t<conjunction<
    different_from_variadic<basic_compressed_slot, Args...>,
    std::is_constructible<T, Args...>
  >::value, int> = 0>
  constexpr basic_compressed_slot(Args&&... args)
    : value_(std::forward<Args>(args)...) {}

  constexpr       T&  value()       &  noexcept { return value_; }
  constexpr const T&  value() const &  noexcept { return value_; }
  constexpr       T&& value()       && noexcept { return std::move(value_); }
  constexpr const T&& value() const && noexcept { return std::move(value_); }

 private:
  T value_;
};

template<typename T>
struct basic_compressed_slot<T, true> : public T {
  template<typename... Args, std::enable_if_t<conjunction<
    different_from_variadic<basic_compressed_slot, Args...>,
    std::is_constructible<T, Args...>
  >::value, int> = 0>
  constexpr basic_compressed_slot(Args&&... args)
    : T(std::forward<Args>(args)...) {}

  constexpr       T&  value()       &  noexcept { return static_cast<      T& >(*this); }
  constexpr const T&  value() const &  noexcept { return static_cast<const T& >(*this); }
  constexpr       T&& value()       && noexcept { return static_cast<      T&&>(*this); }
  constexpr const T&& value() const && noexcept { return static_cast<const T&&>(*this); }
};

template<typename T, std::size_t index>
class compressed_slot : basic_compressed_slot<T> {
  using slot_base = basic_compressed_slot<T>;

 public:
  using slot_base::slot_base;
  using slot_base::value;

  template<std::size_t I> constexpr std::enable_if_t<(I == index),       T&>  get()       &  noexcept { return value(); }
  template<std::size_t I> constexpr std::enable_if_t<(I == index), const T&>  get() const &  noexcept { return value(); }
  template<std::size_t I> constexpr std::enable_if_t<(I == index),       T&&> get()       && noexcept { return std::move(value()); }
  template<std::size_t I> constexpr std::enable_if_t<(I == index), const T&&> get() const && noexcept { return std::move(value()); }
};

} // namespace detail

struct compressed_pair_empty_t {};
PREVIEW_INLINE_VARIABLE constexpr compressed_pair_empty_t compressed_pair_empty;

// A size-optimized pair using empty base optimization
// TODO: Check reference_binds_to_temporary
template<typename T, typename U>
class compressed_pair : public detail::compressed_slot<T, 0>, public detail::compressed_slot<U, 1> {
 private:
  using first_base = detail::compressed_slot<T, 0>;
  using second_base = detail::compressed_slot<U, 1>;

  template<typename Tuple1, typename Tuple2, std::size_t... I1, std::size_t... I2>
  constexpr explicit compressed_pair(std::piecewise_construct_t, Tuple1&& t1, Tuple2&& t2, std::index_sequence<I1...>, std::index_sequence<I2...>)
      : first_base (std::get<I1>(std::forward<Tuple1>(t1))...)
      , second_base(std::get<I2>(std::forward<Tuple2>(t2))...) {}

 public:
  using first_type = T;
  using second_type = U;
  using first_base::get;
  using second_base::get;

  constexpr compressed_pair() = default;

  template<typename T2, std::enable_if_t<conjunction<
      std::is_constructible<T, T2>,
      std::is_constructible<U>
  >::value, int> = 0>
  constexpr compressed_pair(T2&& t, compressed_pair_empty_t)
      : first_base(std::forward<T2>(t)) {}

  template<typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T>,
      std::is_constructible<U, U2>
  >::value, int> = 0>
  constexpr compressed_pair(compressed_pair_empty_t, U2&& u)
      : second_base(std::forward<U2>(u)) {}

  template<typename T2, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T, T2>,
      std::is_constructible<U, U2>,
      std::is_convertible<T2, T>,
      std::is_convertible<U2, U>
  >::value, int> = 0>
  constexpr compressed_pair(T2&& t, U2&& u)
      : first_base(std::forward<T2>(t))
      , second_base(std::forward<U2>(u)) {}

  template<typename T2 = T, typename U2 = U, std::enable_if_t<conjunction<
      std::is_constructible<T, T2>,
      std::is_constructible<U, U2>,
      disjunction<
          negation<std::is_convertible<T2, T>>,
          negation<std::is_convertible<U2, U>>
      >
  >::value, int> = 0>
  constexpr explicit compressed_pair(T2&& t, U2&& u)
      : first_base(std::forward<T2>(t))
      , second_base(std::forward<U2>(u)) {}

  template<typename T2, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T, T2&>,
      std::is_constructible<U, U2&>,
      std::is_convertible<T2&, T>,
      std::is_convertible<U2&, U>
  >::value, int> = 0>
  constexpr compressed_pair(compressed_pair<T2, U2>& p)
      : first_base(p.first())
      , second_base(p.second()) {}

  template<typename T2, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T, T2&>,
      std::is_constructible<U, U2&>,
      disjunction<
          negation<std::is_convertible<T2&, T>>,
          negation<std::is_convertible<U2&, U>>
      >
  >::value, int> = 0>
  constexpr explicit compressed_pair(compressed_pair<T2, U2>& p)
      : first_base(p.first())
      , second_base(p.second()) {}

  template<typename T2, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T, const T2&>,
      std::is_constructible<U, const U2&>,
      std::is_convertible<const T2&, T>,
      std::is_convertible<const U2&, U>
  >::value, int> = 0>
  constexpr compressed_pair(const compressed_pair<T2, U2>& p)
      : first_base(p.first())
      , second_base(p.second()) {}

  template<typename T2, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T, const T2&>,
      std::is_constructible<U, const U2&>,
      disjunction<
          negation<std::is_convertible<const T2&, T>>,
          negation<std::is_convertible<const U2&, U>>
      >
  >::value, int> = 0>
  constexpr explicit compressed_pair(const compressed_pair<T2, U2>& p)
      : first_base(p.first())
      , second_base(p.second()) {}

  template<typename T2, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T, T2>,
      std::is_constructible<U, U2>,
      std::is_convertible<T2, T>,
      std::is_convertible<U2, U>
  >::value, int> = 0>
  constexpr compressed_pair(compressed_pair<T2, U2>&& p)
      : first_base(std::move(p.first()))
      , second_base(std::move(p.second())) {}

  template<typename T2, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T, T2>,
      std::is_constructible<U, U2>,
      disjunction<
          negation<std::is_convertible<T2, T>>,
          negation<std::is_convertible<U2, U>>
      >
  >::value, int> = 0>
  constexpr explicit compressed_pair(compressed_pair<T2, U2>&& p)
      : first_base(std::move(p.first()))
      , second_base(std::move(p.second())) {}

  template<typename T2, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T, const T2>,
      std::is_constructible<U, const U2>,
      std::is_convertible<const T2, T>,
      std::is_convertible<const U2, U>
  >::value, int> = 0>
  constexpr compressed_pair(const compressed_pair<T2, U2>&& p)
      : first_base(std::move(p.first()))
      , second_base(std::move(p.second())) {}

  template<typename T2, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T, const T2>,
      std::is_constructible<U, const U2>,
      disjunction<
          negation<std::is_convertible<const T2, T>>,
          negation<std::is_convertible<const U2, U>>
      >
  >::value, int> = 0>
  constexpr explicit compressed_pair(const compressed_pair<T2, U2>&& p)
      : first_base(std::move(p.first()))
      , second_base(std::move(p.second())) {}

  template<typename P, std::enable_if_t<conjunction<
      pair_like<P>,
      negation<ranges::detail::is_subrange<remove_cvref_t<P>>>,
      std::is_constructible<T, decltype(std::get<0>(std::declval<P>()))>,
      std::is_constructible<U, decltype(std::get<1>(std::declval<P>()))>,
      std::is_convertible<decltype(std::get<0>(std::declval<P>())), T>,
      std::is_convertible<decltype(std::get<1>(std::declval<P>())), U>
  >::value, int> = 0>
  constexpr compressed_pair(P&& u)
      : first_base(std::get<0>(std::forward<P>(u)))
      , second_base(std::get<1>(std::forward<P>(u))) {}

  template<typename P, std::enable_if_t<conjunction<
      pair_like<P>,
      negation<ranges::detail::is_subrange<remove_cvref_t<P>>>,
      std::is_constructible<T, decltype(std::get<0>(std::declval<P>()))>,
      std::is_constructible<U, decltype(std::get<1>(std::declval<P>()))>,
      disjunction<
          negation<std::is_convertible<decltype(std::get<0>(std::declval<P>())), T>>,
          negation<std::is_convertible<decltype(std::get<1>(std::declval<P>())), U>>
      >
  >::value, int> = 0>
  constexpr explicit compressed_pair(P&& u)
      : first_base(std::get<0>(std::forward<P>(u)))
      , second_base(std::get<1>(std::forward<P>(u))) {}

  template<typename Tuple1, typename Tuple2, std::enable_if_t<conjunction<
      tuple_like<Tuple1>,
      tuple_like<Tuple2>,
      detail::is_constructible_from_tuple<T, Tuple1>,
      detail::is_constructible_from_tuple<U, Tuple2>
  >::value, int> = 0>
  constexpr explicit compressed_pair(std::piecewise_construct_t, Tuple1&& first_args, Tuple2&& second_args)
      : compressed_pair(std::piecewise_construct,
                        std::forward<Tuple1>(first_args), std::forward<Tuple2>(second_args),
                        tuple_index_sequence<Tuple1>{}, tuple_index_sequence<Tuple2>{}) {}

  compressed_pair(const compressed_pair&) = default;
  compressed_pair(compressed_pair&&) = default;

  constexpr       T&  first()       &  noexcept { return first_base::value(); }
  constexpr const T&  first() const &  noexcept { return first_base::value(); }
  constexpr       T&& first()       && noexcept { return std::move(first_base::value()); }
  constexpr const T&& first() const && noexcept { return std::move(first_base::value()); }

  constexpr       U&  second()       &  noexcept { return second_base::value(); }
  constexpr const U&  second() const &  noexcept { return second_base::value(); }
  constexpr       U&& second()       && noexcept { return std::move(second_base::value()); }
  constexpr const U&& second() const && noexcept { return std::move(second_base::value()); }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      is_swappable<T>,
      is_swappable<U>
  >::value, int> = 0>
  constexpr void swap(compressed_pair& other)
      noexcept(conjunction<
          is_nothrow_swappable<T>,
          is_nothrow_swappable<U>
      >::value)
  {
    using std::swap;
    swap(first(), other.first());
    swap(second(), other.second());
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      is_swappable<const T>,
      is_swappable<const U>
  >::value, int> = 0>
  constexpr void swap(const compressed_pair& other) const
      noexcept(conjunction<
          is_nothrow_swappable<const T>,
          is_nothrow_swappable<const U>
      >::value)
  {
    using std::swap;
    swap(first(), other.first());
    swap(second(), other.second());
  }
};

template<typename T, typename U, std::enable_if_t<conjunction<
    is_swappable<T>,
    is_swappable<U>
>::value, int> = 0>
constexpr void swap(compressed_pair<T, U>& lhs, compressed_pair<T, U>& rhs)
    noexcept(noexcept(lhs.swap(rhs)))
{
  lhs.swap(rhs);
}

template<typename T, typename U, std::enable_if_t<conjunction<
    is_swappable<const T>,
    is_swappable<const U>
>::value, int> = 0>
constexpr void swap(const compressed_pair<T, U>& lhs, const compressed_pair<T, U>& rhs)
    noexcept(noexcept(lhs.swap(rhs)))
{
  lhs.swap(rhs);
}

template<typename T, typename U>
constexpr compressed_pair<unwrap_ref_decay<T>, unwrap_ref_decay<U>> make_compressed_pair(T&& t, U&& u) {
  return compressed_pair<unwrap_ref_decay<T>, unwrap_ref_decay<U>>(std::forward<T>(t), std::forward<U>(u));
}

} // namespace preview

template<typename T, typename U> PREVIEW_SPECIALIZE_STD_TUPLE_SIZE(preview::compressed_pair<T, U>)
  : public std::integral_constant<std::size_t, 2> {};
template<typename T, typename U> PREVIEW_SPECIALIZE_STD_TUPLE_ELEMENT(0, preview::compressed_pair<T, U>) { using type = T; };
template<typename T, typename U> PREVIEW_SPECIALIZE_STD_TUPLE_ELEMENT(1, preview::compressed_pair<T, U>) { using type = U; };

namespace std {

template<std::size_t I, typename T, typename U>
constexpr       tuple_element_t<I, preview::compressed_pair<T, U>>&  get(      preview::compressed_pair<T, U>&  p) noexcept {
  return p.template get<I>();
}
template<std::size_t I, typename T, typename U>
constexpr const tuple_element_t<I, preview::compressed_pair<T, U>>&  get(const preview::compressed_pair<T, U>&  p) noexcept {
  return p.template get<I>();
}
template<std::size_t I, typename T, typename U>
constexpr       tuple_element_t<I, preview::compressed_pair<T, U>>&& get(      preview::compressed_pair<T, U>&& p) noexcept {
  return std::move(p.template get<I>());
}
template<std::size_t I, typename T, typename U>
constexpr const tuple_element_t<I, preview::compressed_pair<T, U>>&& get(const preview::compressed_pair<T, U>&& p) noexcept {
  return std::move(p.template get<I>());
}

template<typename T, typename U> constexpr       T&  get(      preview::compressed_pair<T, U>&  p) noexcept { return p.first(); }
template<typename T, typename U> constexpr const T&  get(const preview::compressed_pair<T, U>&  p) noexcept { return p.first(); }
template<typename T, typename U> constexpr       T&& get(      preview::compressed_pair<T, U>&& p) noexcept { return std::move(p.first()); }
template<typename T, typename U> constexpr const T&& get(const preview::compressed_pair<T, U>&& p) noexcept { return std::move(p.first()); }

template<typename T, typename U> constexpr       T&  get(      preview::compressed_pair<U, T>&  p) noexcept { return p.second(); }
template<typename T, typename U> constexpr const T&  get(const preview::compressed_pair<U, T>&  p) noexcept { return p.second(); }
template<typename T, typename U> constexpr       T&& get(      preview::compressed_pair<U, T>&& p) noexcept { return std::move(p.second()); }
template<typename T, typename U> constexpr const T&& get(const preview::compressed_pair<U, T>&& p) noexcept { return std::move(p.second()); }

} // namespace std

#endif // PREVIEW_UTILITY_COMPRESSED_PAIR_H_
