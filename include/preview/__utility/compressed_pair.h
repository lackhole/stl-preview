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
# include "preview/__type_traits/is_implicitly_default_constructible.h"
# include "preview/__type_traits/unwrap_reference.h"
# include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace detail {

template<typename T, bool = /* false */ std::is_empty<T>::value>
struct basic_compressed_slot {
  template<bool B = std::is_default_constructible<T>::value, std::enable_if_t<B, int> = 0>
  constexpr basic_compressed_slot() : value_() {}

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
  template<bool B = std::is_default_constructible<T>::value, std::enable_if_t<B, int> = 0>
  constexpr basic_compressed_slot() : T() {}

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
template<typename T1, typename T2>
class compressed_pair : public detail::compressed_slot<T1, 0>, public detail::compressed_slot<T2, 1> {
 private:
  using first_base = detail::compressed_slot<T1, 0>;
  using second_base = detail::compressed_slot<T2, 1>;

  template<typename Tuple1, typename Tuple2, std::size_t... I1, std::size_t... I2>
  constexpr explicit compressed_pair(std::piecewise_construct_t, Tuple1&& t1, Tuple2&& t2, std::index_sequence<I1...>, std::index_sequence<I2...>)
      : first_base (std::get<I1>(std::forward<Tuple1>(t1))...)
      , second_base(std::get<I2>(std::forward<Tuple2>(t2))...) {}

 public:
  using first_type = T1;
  using second_type = T2;
  using first_base::get;
  using second_base::get;

  template<typename Dummy = void, std::enable_if_t<conjunction_v<std::is_void<Dummy>,
      std::is_default_constructible<T1>,
      std::is_default_constructible<T2>,
      /* explicit(true) */
      disjunction<
          negation<is_implicitly_default_constructible<T1>>,
          negation<is_implicitly_default_constructible<T2>>>
  >, int> = 0>
  constexpr explicit compressed_pair()
      : first_base()
      , second_base() {}

  template<typename Dummy = void, std::enable_if_t<conjunction_v<std::is_void<Dummy>,
      std::is_default_constructible<T1>,
      std::is_default_constructible<T2>,
      /* explicit(false) */
      is_implicitly_default_constructible<T1>,
      is_implicitly_default_constructible<T2>
  >, int> = 0>
  constexpr compressed_pair()
      : first_base()
      , second_base() {}

  template<typename Dummy = void, std::enable_if_t<conjunction_v<std::is_void<Dummy>,
      std::is_copy_constructible<T1>,
      std::is_copy_constructible<T2>,
      /* explicit(true) */
      disjunction<
          negation<std::is_convertible<const T1&, T1>>,
          negation<std::is_convertible<const T2&, T2>>>
  >, int> = 0>
  constexpr explicit compressed_pair(const T1& x,  const T2& y)
      : first_base(x)
      , second_base(y) {}

  template<typename Dummy = void, std::enable_if_t<conjunction_v<std::is_void<Dummy>,
      std::is_copy_constructible<T1>,
      std::is_copy_constructible<T2>,
      /* explicit(false) */
      std::is_convertible<const T1&, T1>,
      std::is_convertible<const T2&, T2>
  >, int> = 0>
  constexpr compressed_pair(const T1& x,  const T2& y)
      : first_base(x)
      , second_base(y) {}

  template<typename U1 = T1, typename U2 = T2, std::enable_if_t<conjunction<
      std::is_constructible<T1, U1>,
      std::is_constructible<T2, U2>,
      /* explicit(true) */
      disjunction<
          negation<std::is_convertible<U1, T1>>,
          negation<std::is_convertible<U2, T2>>
      >
      // TODO: Add reference_binds_to_temporary check
  >::value, int> = 0>
  constexpr explicit compressed_pair(U1&& x, U2&& y)
      : first_base(std::forward<U1>(x))
      , second_base(std::forward<U2>(y)) {}

  template<typename U1 = T1, typename U2 = T2, std::enable_if_t<conjunction<
      std::is_constructible<T1, U1>,
      std::is_constructible<T2, U2>,
      /* explicit(false) */
      std::is_convertible<U1, T1>,
      std::is_convertible<U2, T2>
      // TODO: Add reference_binds_to_temporary check
  >::value, int> = 0>
  constexpr compressed_pair(U1&& x, U2&& y)
      : first_base(std::forward<U1>(x))
      , second_base(std::forward<U2>(y)) {}

  template<typename U1, std::enable_if_t<conjunction<
      std::is_constructible<T1, U1>,
      std::is_constructible<T2>
  >::value, int> = 0>
  constexpr compressed_pair(U1&& x, compressed_pair_empty_t)
      : first_base(std::forward<U1>(x)) {}

  template<typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T1>,
      std::is_constructible<T2, U2>
  >::value, int> = 0>
  constexpr compressed_pair(compressed_pair_empty_t, U2&& y)
      : second_base(std::forward<U2>(y)) {}

  template<typename U1, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T1, U1&>,
      std::is_constructible<T2, U2&>,
      /* explicit(true) */
      disjunction<
          negation<std::is_convertible<U1&, T1>>,
          negation<std::is_convertible<U2&, T2>>
      >
  >::value, int> = 0>
  constexpr explicit compressed_pair(compressed_pair<U1, U2>& p)
      : first_base(p.first())
      , second_base(p.second()) {}

  template<typename U1, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T1, U1&>,
      std::is_constructible<T2, U2&>,
      /* explicit(false) */
      std::is_convertible<U1&, T1>,
      std::is_convertible<U2&, T2>
  >::value, int> = 0>
  constexpr compressed_pair(compressed_pair<U1, U2>& p)
      : first_base(p.first())
      , second_base(p.second()) {}

  template<typename U1, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T1, const U1&>,
      std::is_constructible<T2, const U2&>,
      /* explicit(true) */
      disjunction<
          negation<std::is_convertible<const U1&, T1>>,
          negation<std::is_convertible<const U2&, T2>>
      >
  >::value, int> = 0>
  constexpr explicit compressed_pair(const compressed_pair<U1, U2>& p)
      : first_base(p.first())
      , second_base(p.second()) {}

  template<typename U1, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T1, const U1&>,
      std::is_constructible<T2, const U2&>,
      /* explicit(false) */
      std::is_convertible<const U1&, T1>,
      std::is_convertible<const U2&, T2>
  >::value, int> = 0>
  constexpr compressed_pair(const compressed_pair<U1, U2>& p)
      : first_base(p.first())
      , second_base(p.second()) {}

  template<typename U1, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T1, U1>,
      std::is_constructible<T2, U2>,
      /* explicit(true) */
      disjunction<
          negation<std::is_convertible<U1, T1>>,
          negation<std::is_convertible<U2, T2>>
      >
  >::value, int> = 0>
  constexpr explicit compressed_pair(compressed_pair<U1, U2>&& p)
      : first_base(std::move(p.first()))
      , second_base(std::move(p.second())) {}

  template<typename U1, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T1, U1>,
      std::is_constructible<T2, U2>,
      /* explicit(false) */
      std::is_convertible<U1, T1>,
      std::is_convertible<U2, T2>
  >::value, int> = 0>
  constexpr compressed_pair(compressed_pair<U1, U2>&& p)
      : first_base(std::move(p.first()))
      , second_base(std::move(p.second())) {}

  template<typename U1, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T1, const U1>,
      std::is_constructible<T2, const U2>,
      /* explicit(true) */
      disjunction<
          negation<std::is_convertible<const U1, T1>>,
          negation<std::is_convertible<const U2, T2>>
      >
  >::value, int> = 0>
  constexpr explicit compressed_pair(const compressed_pair<U1, U2>&& p)
      : first_base(std::move(p.first()))
      , second_base(std::move(p.second())) {}

  template<typename U1, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T1, const U1>,
      std::is_constructible<T2, const U2>,
      /* explicit(false) */
      std::is_convertible<const U1, T1>,
      std::is_convertible<const U2, T2>
  >::value, int> = 0>
  constexpr compressed_pair(const compressed_pair<U1, U2>&& p)
      : first_base(std::move(p.first()))
      , second_base(std::move(p.second())) {}

  template<typename P, std::enable_if_t<conjunction<
      pair_like<P>,
      negation<ranges::detail::is_subrange<remove_cvref_t<P>>>,
      std::is_constructible<T1, decltype(std::get<0>(std::declval<P>()))>,
      std::is_constructible<T2, decltype(std::get<1>(std::declval<P>()))>,
      /* explicit(true) */
      disjunction<
          negation<std::is_convertible<decltype(std::get<0>(std::declval<P>())), T1>>,
          negation<std::is_convertible<decltype(std::get<1>(std::declval<P>())), T2>>
      >
  >::value, int> = 0>
  constexpr explicit compressed_pair(P&& u)
      : first_base(std::get<0>(std::forward<P>(u)))
      , second_base(std::get<1>(std::forward<P>(u))) {}

  template<typename P, std::enable_if_t<conjunction<
      pair_like<P>,
      negation<ranges::detail::is_subrange<remove_cvref_t<P>>>,
      std::is_constructible<T1, decltype(std::get<0>(std::declval<P>()))>,
      std::is_constructible<T2, decltype(std::get<1>(std::declval<P>()))>,
      /* explicit(false) */
      std::is_convertible<decltype(std::get<0>(std::declval<P>())), T1>,
      std::is_convertible<decltype(std::get<1>(std::declval<P>())), T2>
  >::value, int> = 0>
  constexpr compressed_pair(P&& u)
      : first_base(std::get<0>(std::forward<P>(u)))
      , second_base(std::get<1>(std::forward<P>(u))) {}

  template<typename Tuple1, typename Tuple2, std::enable_if_t<conjunction<
      tuple_like<Tuple1>,
      tuple_like<Tuple2>,
      detail::is_constructible_from_tuple<T1, Tuple1>,
      detail::is_constructible_from_tuple<T2, Tuple2>
  >::value, int> = 0>
  constexpr explicit compressed_pair(std::piecewise_construct_t, Tuple1&& first_args, Tuple2&& second_args)
      : compressed_pair(std::piecewise_construct,
                        std::forward<Tuple1>(first_args), std::forward<Tuple2>(second_args),
                        tuple_index_sequence<Tuple1>{}, tuple_index_sequence<Tuple2>{}) {}

  compressed_pair(const compressed_pair&) = default;
  compressed_pair(compressed_pair&&) = default;

  template<typename U1, typename U2, std::enable_if_t<conjunction_v<
      std::is_assignable<T1&, const U1&>,
      std::is_assignable<T2&, const U2&>
  >, int> = 0>
  constexpr compressed_pair& operator=(const compressed_pair<U1, U2>& p) {
    first() = p.first();
    second() = p.second();
    return *this;
  }

  template<typename U1, typename U2, std::enable_if_t<conjunction_v<
      std::is_assignable<T1&, U1>,
      std::is_assignable<T2&, U2>
  >, int> = 0>
  constexpr compressed_pair& operator=(compressed_pair<U1, U2>&& p) {
    first() = std::forward<U1>(p.first());
    second() = std::forward<U2>(p.second());
    return *this;
  }

  template<typename P, std::enable_if_t<conjunction_v<
      pair_like<P>,
      different_from<P, compressed_pair>,
      negation<ranges::detail::is_subrange<remove_cvref_t<P>>>,
      std::is_assignable<T1&, decltype(std::get<0>(std::declval<P>()))>,
      std::is_assignable<T2&, decltype(std::get<1>(std::declval<P>()))>
  >, int> = 0>
  constexpr compressed_pair& operator=(P&& p) {
    first() = std::get<0>(std::forward<P>(p));
    second() = std::get<1>(std::forward<P>(p));
    return *this;
  }

  constexpr       T1&  first()       &  noexcept { return first_base::value(); }
  constexpr const T1&  first() const &  noexcept { return first_base::value(); }
  constexpr       T1&& first()       && noexcept { return std::move(first_base::value()); }
  constexpr const T1&& first() const && noexcept { return std::move(first_base::value()); }

  constexpr       T2&  second()       &  noexcept { return second_base::value(); }
  constexpr const T2&  second() const &  noexcept { return second_base::value(); }
  constexpr       T2&& second()       && noexcept { return std::move(second_base::value()); }
  constexpr const T2&& second() const && noexcept { return std::move(second_base::value()); }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      is_swappable<T1>,
      is_swappable<T2>
  >::value, int> = 0>
  constexpr void swap(compressed_pair& other)
      noexcept(conjunction<
          is_nothrow_swappable<T1>,
          is_nothrow_swappable<T2>
      >::value)
  {
    using std::swap;
    swap(first(), other.first());
    swap(second(), other.second());
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      is_swappable<const T1>,
      is_swappable<const T2>
  >::value, int> = 0>
  constexpr void swap(const compressed_pair& other) const
      noexcept(conjunction<
          is_nothrow_swappable<const T1>,
          is_nothrow_swappable<const T2>
      >::value)
  {
    using std::swap;
    swap(first(), other.first());
    swap(second(), other.second());
  }
};

template<typename T1, typename T2, std::enable_if_t<conjunction<
    is_swappable<T1>,
    is_swappable<T2>
>::value, int> = 0>
constexpr void swap(compressed_pair<T1, T2>& lhs, compressed_pair<T1, T2>& rhs)
    noexcept(noexcept(lhs.swap(rhs)))
{
  lhs.swap(rhs);
}

template<typename T1, typename T2, std::enable_if_t<conjunction<
    is_swappable<const T1>,
    is_swappable<const T2>
>::value, int> = 0>
constexpr void swap(const compressed_pair<T1, T2>& lhs, const compressed_pair<T1, T2>& rhs)
    noexcept(noexcept(lhs.swap(rhs)))
{
  lhs.swap(rhs);
}

template<typename T1, typename T2, typename U1, typename U2, std::enable_if_t<conjunction_v<
    rel_ops::is_equality_comparable<const T1&, const U1&>,
    rel_ops::is_equality_comparable<const T2&, const U2&>
>, int> = 0>
constexpr bool operator==(const compressed_pair<T1, T2>& x, const compressed_pair<U1, U2>& y) {
  return x.first() == y.first() && x.second() == y.second();
}

template<typename T1, typename T2, typename U1, typename U2, std::enable_if_t<conjunction_v<
    rel_ops::is_equality_comparable<const T1&, const U1&>,
    rel_ops::is_equality_comparable<const T2&, const U2&>
>, int> = 0>
constexpr bool operator!=(const compressed_pair<T1, T2>& x, const compressed_pair<U1, U2>& y) {
  return !(x == y);
}

template<typename T1, typename T2, typename U1, typename U2>
constexpr bool operator<(const compressed_pair<T1, T2>& x, const compressed_pair<U1, U2>& y) {
  return x.first() < y.first() || (!(y.first() < x.first()) && x.second() < y.second());
}

template<typename T1, typename T2, typename U1, typename U2>
constexpr bool operator>(const compressed_pair<T1, T2>& x, const compressed_pair<U1, U2>& y) {
  return y < x;
}

template<typename T1, typename T2, typename U1, typename U2>
constexpr bool operator<=(const compressed_pair<T1, T2>& x, const compressed_pair<U1, U2>& y) {
  return !(y < x);
}

template<typename T1, typename T2, typename U1, typename U2>
constexpr bool operator>=(const compressed_pair<T1, T2>& x, const compressed_pair<U1, U2>& y) {
  return !(x < y);
}

template<typename T1, typename T2>
constexpr compressed_pair<unwrap_ref_decay<T1>, unwrap_ref_decay<T2>> make_compressed_pair(T1&& t, T2&& u) {
  return compressed_pair<unwrap_ref_decay<T1>, unwrap_ref_decay<T2>>(std::forward<T1>(t), std::forward<T2>(u));
}

} // namespace preview

template<typename T1, typename T2> PREVIEW_SPECIALIZE_STD_TUPLE_SIZE(preview::compressed_pair<T1, T2>)
  : public std::integral_constant<std::size_t, 2> {};
template<typename T1, typename T2> PREVIEW_SPECIALIZE_STD_TUPLE_ELEMENT(0, preview::compressed_pair<T1, T2>) { using type = T1; };
template<typename T1, typename T2> PREVIEW_SPECIALIZE_STD_TUPLE_ELEMENT(1, preview::compressed_pair<T1, T2>) { using type = T2; };

namespace std {

template<std::size_t I, typename T1, typename T2>
constexpr       tuple_element_t<I, preview::compressed_pair<T1, T2>>&  get(      preview::compressed_pair<T1, T2>&  p) noexcept {
  return p.template get<I>();
}
template<std::size_t I, typename T1, typename T2>
constexpr const tuple_element_t<I, preview::compressed_pair<T1, T2>>&  get(const preview::compressed_pair<T1, T2>&  p) noexcept {
  return p.template get<I>();
}
template<std::size_t I, typename T1, typename T2>
constexpr       tuple_element_t<I, preview::compressed_pair<T1, T2>>&& get(      preview::compressed_pair<T1, T2>&& p) noexcept {
  return std::move(p.template get<I>());
}
template<std::size_t I, typename T1, typename T2>
constexpr const tuple_element_t<I, preview::compressed_pair<T1, T2>>&& get(const preview::compressed_pair<T1, T2>&& p) noexcept {
  return std::move(p.template get<I>());
}

template<typename T1, typename T2> constexpr       T1&  get(      preview::compressed_pair<T1, T2>&  p) noexcept { return p.first(); }
template<typename T1, typename T2> constexpr const T1&  get(const preview::compressed_pair<T1, T2>&  p) noexcept { return p.first(); }
template<typename T1, typename T2> constexpr       T1&& get(      preview::compressed_pair<T1, T2>&& p) noexcept { return std::move(p.first()); }
template<typename T1, typename T2> constexpr const T1&& get(const preview::compressed_pair<T1, T2>&& p) noexcept { return std::move(p.first()); }

template<typename T1, typename T2> constexpr       T1&  get(      preview::compressed_pair<T2, T1>&  p) noexcept { return p.second(); }
template<typename T1, typename T2> constexpr const T1&  get(const preview::compressed_pair<T2, T1>&  p) noexcept { return p.second(); }
template<typename T1, typename T2> constexpr       T1&& get(      preview::compressed_pair<T2, T1>&& p) noexcept { return std::move(p.second()); }
template<typename T1, typename T2> constexpr const T1&& get(const preview::compressed_pair<T2, T1>&& p) noexcept { return std::move(p.second()); }

} // namespace std

#endif // PREVIEW_UTILITY_COMPRESSED_PAIR_H_
