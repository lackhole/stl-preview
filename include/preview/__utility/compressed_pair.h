# /*
#  * Created by YongGyu Lee on 2021/11/14.
#  */
#
# ifndef PREVIEW_UTILITY_COMPRESSED_PAIR_H_
# define PREVIEW_UTILITY_COMPRESSED_PAIR_H_
#
# include <cstddef>
# include <type_traits>
# include <utility>
#
# include "preview/__core/inline_variable.h"
# include "preview/__tuple/specialize_tuple.h"
# include "preview/__type_traits/conjunction.h"
# include "preview/__type_traits/is_swappable.h"

namespace preview {

namespace detail {

template<typename T, std::size_t index, bool v = std::is_empty<T>::value /* false */>
struct compressed_slot {
  constexpr compressed_slot() = default;

  template<typename U, std::enable_if_t<std::is_same<std::decay_t<U>, compressed_slot>::value == false, int> = 0>
  constexpr compressed_slot(U&& u) : value_(std::forward<U>(u)) {}

  template<std::size_t I> constexpr std::enable_if_t<(I == index), T&>        get() & noexcept { return value_; }
  template<std::size_t I> constexpr std::enable_if_t<(I == index), T&&>       get() && noexcept { return std::move(value_); }
  template<std::size_t I> constexpr std::enable_if_t<(I == index), const T&>  get() const & noexcept { return value_; }
  template<std::size_t I> constexpr std::enable_if_t<(I == index), const T&&> get() const && noexcept { return std::move(value_); }

 private:
  T value_;
};

template<typename T, std::size_t index>
struct compressed_slot<T, index, true> : public T {
  constexpr compressed_slot() = default;

  template<typename U, std::enable_if_t<std::is_same<std::decay_t<U>, compressed_slot>::value == false, int> = 0>
  constexpr compressed_slot(U&& u) : T(std::forward<U>(u)) {}

  template<std::size_t I> constexpr std::enable_if_t<(I == index), T&> get() & noexcept { return static_cast<T&>(*this); }
  template<std::size_t I> constexpr std::enable_if_t<(I == index), T&&> get() && noexcept { return static_cast<T&&>(*this); }
  template<std::size_t I> constexpr std::enable_if_t<(I == index), const T&> get() const & noexcept { return static_cast<const T&>(*this); }
  template<std::size_t I> constexpr std::enable_if_t<(I == index), const T&&> get() const && noexcept { return static_cast<const T&&>(*this); }
};

} // namespace detail

struct compressed_pair_empty_t {};
PREVIEW_INLINE_VARIABLE constexpr compressed_pair_empty_t compressed_pair_empty;


// An size-optimized pair using empty base optimization
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

  constexpr T& first() & noexcept { return first_base::template get<0>(); }
  constexpr T&& first() && noexcept { return std::move(first_base::template get<0>()); }
  constexpr const T& first() const & noexcept { return first_base::template get<0>(); }
  constexpr const T&& first() const && noexcept { return std::move(first_base::template get<0>()); }

  constexpr U& second() & noexcept { return second_base::template get<1>(); }
  constexpr U&& second() && noexcept { return std::move(second_base::template get<1>()); }
  constexpr const U& second() const & noexcept { return second_base::template get<1>(); }
  constexpr const U&& second() const && noexcept { return std::move(second_base::template get<1>()); }

  constexpr std::enable_if_t<conjunction<is_swappable<T>, is_swappable<U>>::value>
  swap(compressed_pair& other)
      noexcept(conjunction<is_nothrow_swappable<T>, is_nothrow_swappable<U>>::value)
  {
    using std::swap;
    swap(first(), other.first());
    swap(second(), other.second());
  }
};


template<typename T, typename U>
constexpr std::enable_if_t<conjunction<is_swappable<T>, is_swappable<U>>::value>
swap(compressed_pair<T, U>& lhs, compressed_pair<T, U>& rhs)
    noexcept(conjunction<is_nothrow_swappable<T>, is_nothrow_swappable<U>>::value)
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
  : std::integral_constant<std::size_t, 2> {};
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
