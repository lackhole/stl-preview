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
# include "preview/__tuple/specialize_tuple.h"
# include "preview/__type_traits/common_type.h"
# include "preview/__type_traits/conjunction.h"
# include "preview/__type_traits/is_swappable.h"
# include "preview/__utility/integer_sequence.h"
# include "preview/__utility/in_place.h"

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
  constexpr basic_compressed_slot() = default;

  template<typename Arg, typename... Args, std::enable_if_t<conjunction<
    different_from<basic_compressed_slot, Arg>,
    std::is_constructible<T, Arg, Args...>
  >::value, int> = 0>
  constexpr basic_compressed_slot(Arg&& arg, Args&&... args)
    : T(std::forward<Arg>(arg), std::forward<Args>(args)...) {}

  constexpr       T&  value()       &  noexcept { return static_cast<      T& >(*this); }
  constexpr const T&  value() const &  noexcept { return static_cast<const T& >(*this); }
  constexpr       T&& value()       && noexcept { return static_cast<      T&&>(*this); }
  constexpr const T&& value() const && noexcept { return static_cast<const T&&>(*this); }
};

template<typename T, typename IndexSequence, typename Tuple>
struct tuple_index_constructible_from;
template<typename T, std::size_t... I, typename Tuple>
struct tuple_index_constructible_from<T, std::index_sequence<I...>, Tuple>
  : std::is_constructible<T, std::tuple_element_t<I, Tuple>...> {};

template<typename T, std::size_t index>
class compressed_slot : basic_compressed_slot<T> {
  using slot_base = basic_compressed_slot<T>;
  using slot_base::value;

 public:
  using slot_base::slot_base;

  template<typename U, std::size_t... I>
  constexpr explicit compressed_slot(std::piecewise_construct_t, std::index_sequence<I...>, U&& ArgTuple)
      : slot_base(std::get<I>(std::forward<U>(ArgTuple))...) {}

  template<std::size_t I> constexpr std::enable_if_t<(I == index),       T&>  get()       &  noexcept { return value(); }
  template<std::size_t I> constexpr std::enable_if_t<(I == index), const T&>  get() const &  noexcept { return value(); }
  template<std::size_t I> constexpr std::enable_if_t<(I == index),       T&&> get()       && noexcept { return std::move(value()); }
  template<std::size_t I> constexpr std::enable_if_t<(I == index), const T&&> get() const && noexcept { return std::move(value()); }
};

} // namespace detail

struct compressed_pair_empty_t {};
PREVIEW_INLINE_VARIABLE constexpr compressed_pair_empty_t compressed_pair_empty;

template<std::size_t I>
struct compressed_pair_variadic_divider_t {};

// A size-optimized pair using empty base optimization
template<typename T, typename U>
class compressed_pair : public detail::compressed_slot<T, 0>, public detail::compressed_slot<U, 1> {
 private:
  using first_base = detail::compressed_slot<T, 0>;
  using second_base = detail::compressed_slot<U, 1>;

 public:
  using first_type = T;
  using second_type = U;
  using first_base::get;
  using second_base::get;

  constexpr compressed_pair() = default;

  template<typename T2, std::enable_if_t<std::is_constructible<T, T2>::value, int> = 0>
  constexpr compressed_pair(T2&& t, compressed_pair_empty_t) : first_base(std::forward<T2>(t)) {}

  template<typename U2, std::enable_if_t<std::is_constructible<U, U2>::value, int> = 0>
  constexpr compressed_pair(compressed_pair_empty_t, U2&& u) : second_base(std::forward<U2>(u)) {}

  template<typename T2, typename U2, std::enable_if_t<conjunction<
      std::is_constructible<T, T2>,
      std::is_constructible<U, U2>
  >::value, int> = 0>
  constexpr compressed_pair(T2&& t, U2&& u) : first_base(std::forward<T2>(t)), second_base(std::forward<U2>(u)) {}

  template<std::size_t N, typename... Args, std::enable_if_t<conjunction<
      detail::tuple_index_constructible_from<T, make_index_sequence<0, N              >, std::tuple<Args&&...>>,
      detail::tuple_index_constructible_from<U, make_index_sequence<N, sizeof...(Args)>, std::tuple<Args&&...>>
  >::value, int> = 0>
  constexpr explicit compressed_pair(compressed_pair_variadic_divider_t<N>, Args&&... args)
      : first_base (std::piecewise_construct, make_index_sequence<0, N              >{}, std::forward_as_tuple(args...))
      , second_base(std::piecewise_construct, make_index_sequence<N, sizeof...(Args)>{}, std::forward_as_tuple(args...)) {}

  constexpr T& first() & noexcept { return first_base::template get<0>(); }
  constexpr T&& first() && noexcept { return std::move(first_base::template get<0>()); }
  constexpr const T& first() const & noexcept { return first_base::template get<0>(); }
  constexpr const T&& first() const && noexcept { return std::move(first_base::template get<0>()); }

  constexpr U& second() & noexcept { return second_base::template get<1>(); }
  constexpr U&& second() && noexcept { return std::move(second_base::template get<1>()); }
  constexpr const U& second() const & noexcept { return second_base::template get<1>(); }
  constexpr const U&& second() const && noexcept { return std::move(second_base::template get<1>()); }

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

namespace detail {

template<std::size_t I> struct compressed_pair_getter;
template<>
struct compressed_pair_getter<0> {
  template<typename T, typename U> static constexpr T& get(compressed_pair<T, U>& p) noexcept { return p.first(); }
  template<typename T, typename U> static constexpr const T& get(const compressed_pair<T, U>& p) noexcept { return p.first(); }
  template<typename T, typename U> static constexpr T&& get(compressed_pair<T, U>&& p) noexcept { return std::move(p.first()); }
  template<typename T, typename U> static constexpr const T&& get(const compressed_pair<T, U>&& p) noexcept { return std::move(p.first()); }
};
template<>
struct compressed_pair_getter<1> {
  template<typename T, typename U> static constexpr U& get(compressed_pair<T, U>& p) noexcept { return p.second(); }
  template<typename T, typename U> static constexpr const U& get(const compressed_pair<T, U>& p) noexcept { return p.second(); }
  template<typename T, typename U> static constexpr U&& get(compressed_pair<T, U>&& p) noexcept { return std::move(p.second()); }
  template<typename T, typename U> static constexpr const U&& get(const compressed_pair<T, U>&& p) noexcept { return std::move(p.second()); }
};

} // namespace detail

} // namespace preview

template<typename T, typename U> PREVIEW_SPECIALIZE_STD_TUPLE_SIZE(preview::compressed_pair<T, U>)
  : public std::integral_constant<std::size_t, 2> {};
template<typename T, typename U> PREVIEW_SPECIALIZE_STD_TUPLE_ELEMENT(0, preview::compressed_pair<T, U>) { using type = T; };
template<typename T, typename U> PREVIEW_SPECIALIZE_STD_TUPLE_ELEMENT(1, preview::compressed_pair<T, U>) { using type = U; };

namespace std {

template<std::size_t I, typename T, typename U>
constexpr tuple_element_t<I, preview::compressed_pair<T, U>>& get(preview::compressed_pair<T, U>& p) noexcept {
  return preview::detail::compressed_pair_getter<I>::get(p);
}
template<std::size_t I, typename T, typename U>
constexpr const tuple_element_t<I, preview::compressed_pair<T, U>>& get(const preview::compressed_pair<T, U>& p) noexcept {
  return preview::detail::compressed_pair_getter<I>::get(p);
}
template<std::size_t I, typename T, typename U>
constexpr tuple_element_t<I, preview::compressed_pair<T, U>>&& get(preview::compressed_pair<T, U>&& p) noexcept {
  return preview::detail::compressed_pair_getter<I>::get(std::move(p));
}
template<std::size_t I, typename T, typename U>
constexpr const tuple_element_t<I, preview::compressed_pair<T, U>>&& get(const preview::compressed_pair<T, U>&& p) noexcept {
  return preview::detail::compressed_pair_getter<I>::get(std::move(p));
}

template<typename T, typename U> constexpr T& get(preview::compressed_pair<T, U>& p) noexcept { return p.first(); }
template<typename T, typename U> constexpr const T& get(const preview::compressed_pair<T, U>& p) noexcept { return p.first(); }
template<typename T, typename U> constexpr T&& get(preview::compressed_pair<T, U>&& p) noexcept { return std::move(p.first()); }
template<typename T, typename U> constexpr const T&& get(const preview::compressed_pair<T, U>&& p) noexcept { return std::move(p.first()); }

template<typename T, typename U> constexpr T& get(preview::compressed_pair<U, T>& p) noexcept { return p.second(); }
template<typename T, typename U> constexpr const T& get(const preview::compressed_pair<U, T>& p) noexcept { return p.second(); }
template<typename T, typename U> constexpr T&& get(preview::compressed_pair<U, T>&& p) noexcept { return std::move(p.second()); }
template<typename T, typename U> constexpr const T&& get(const preview::compressed_pair<U, T>&& p) noexcept { return std::move(p.second()); }

// template<std::size_t I, typename T, typename U>


} // namespace std

#endif // PREVIEW_UTILITY_COMPRESSED_PAIR_H_
