//
// Created by yonggyulee on 2023/12/26.
//

#ifndef PREVIEW_RANGES_VIEWS_IOTA_H_
#define PREVIEW_RANGES_VIEWS_IOTA_H_

#include <cstdint>
#include <type_traits>

#include "preview/core.h"
#include "preview/__concepts/copyable.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__concepts/default_initializable.h"
#include "preview/__concepts/requires_expression.h"
#include "preview/__concepts/same_as.h"
#include "preview/__concepts/semiregular.h"
#include "preview/__concepts/totally_ordered.h"
#include "preview/__concepts/weakly_equality_comparable_with.h"
#include "preview/__core/inline_variable.h"
#include "preview/__iterator/detail/have_cxx20_iterator.h"
#include "preview/__iterator/incrementable.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/random_access_iterator.h"
#include "preview/__iterator/unreachable_sentinel.h"
#include "preview/__iterator/weakly_incrementable.h"
#include "preview/__ranges/borrowed_range.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/type_identity.h"
#include "preview/__type_traits/void_t.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename I>
struct iota_diff
    : std::conditional_t<
        disjunction<
          negation<std::is_integral<I>>,
          conjunction<
            std::is_integral<I>,
            bool_constant<( sizeof(iter_difference_t<I>) > sizeof(I) )>
          >
        >::value,
        type_identity<iter_difference_t<I>>,
        type_identity<std::intmax_t>
      > {};

template<typename I>
using iota_diff_t = typename iota_diff<I>::type;

template<typename I, bool = incrementable<I>::value, typename = void, typename = void>
struct iv_decrementable : std::false_type {};

template<typename I>
struct iv_decrementable<
      I, true,
      void_t<decltype( --std::declval<I&>() )>,
      void_t<decltype( std::declval<I&>()-- )>
    > : conjunction<
          same_as<decltype( --std::declval<I&>() ), I&>,
          same_as<decltype( std::declval<I&>()-- ), I>
        > {};

template<typename I, typename D, typename = void, typename = void, typename = void, typename = void>
struct iv_advanceable_explicit : std::false_type {};

template<typename I, typename D>
struct iv_advanceable_explicit<
      I, D,
      void_t<decltype( std::declval<I>() + std::declval<D>() )>,
      void_t<decltype( std::declval<D>() + std::declval<I>() )>,
      void_t<decltype( std::declval<I>() - std::declval<D>() )>,
      void_t<decltype( std::declval<I>() - std::declval<I>() )>
    > : conjunction<
          std::is_constructible<remove_cvref_t<I>, decltype( std::declval<I>() + std::declval<D>() )>,
          std::is_constructible<remove_cvref_t<I>, decltype( std::declval<D>() + std::declval<I>() )>,
          std::is_constructible<remove_cvref_t<I>, decltype( std::declval<I>() - std::declval<D>() )>,
          convertible_to<decltype( std::declval<I>() - std::declval<I>() ), remove_cvref_t<D>>
        >{};

template<typename I, bool = conjunction<iv_decrementable<I>, totally_ordered<I>>::value>
struct iv_advanceable : std::false_type {};

// TODO: Implement
template<typename I>
struct iv_advanceable<I, true>
    : conjunction<
        requires_expression<preview::detail::detail_random_access_iterator::explicit_op_assign_check, I&, const iota_diff_t<I>&>,
        requires_expression<iv_advanceable_explicit, const I&, const iota_diff_t<I>&>
    > {};

template<typename I, bool = incrementable<I>::value /* true */>
struct iota_view_iterator_category {
  using iterator_category = input_iterator_tag;
};
template<typename I>
struct iota_view_iterator_category<I, false> {};

template<typename I>
struct iota_view_iterator_concept
    : conditional_t<
        iv_advanceable<I>, type_identity<random_access_iterator_tag>,
        iv_decrementable<I>, type_identity<bidirectional_iterator_tag>,
        incrementable<I>, type_identity<forward_iterator_tag>,
        type_identity<input_iterator_tag>
      > {};

template<typename W, typename Bound, typename IV>
struct iv_ctor_iterator_last {
  using type = typename IV::sentinel;
};

template<typename W, typename IV>
struct iv_ctor_iterator_last<W, unreachable_sentinel_t, IV> {
  using type = unreachable_sentinel_t;
};

template<typename W, typename IV>
struct iv_ctor_iterator_last<W, W, IV> {
  using type = typename IV::iterator;
};

} // namespace detail

template<typename W,
         typename Bound = unreachable_sentinel_t>
class iota_view : public view_interface<iota_view<W, Bound>> {
 public:
  static_assert(weakly_incrementable<W>::value, "Constraint not satisfied");
  static_assert(semiregular<Bound>::value, "Constraint not satisfied");
  static_assert(weakly_equality_comparable_with<W, Bound>::value, "Constraint not satisfied");
  static_assert(copyable<W>::value, "Constraint not satisfied");

  class iterator;
  class sentinel;

  class iterator : public detail::iota_view_iterator_category<W> {
   public:
    using value_type = W;
    using difference_type = detail::iota_diff_t<W>;
    using iterator_concept = typename detail::iota_view_iterator_concept<W>::type;

#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
    using pointer = void;
    using reference = std::conditional_t<incrementable<W>::value, W, void>;
#endif

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      default_initializable<W>
    >::value, int> = 0>
    constexpr iterator() : value_(W()) {}

    constexpr explicit iterator(W value) : value_(value) {}

    constexpr W operator*() const noexcept(std::is_nothrow_copy_constructible<W>::value) {
      return value_;
    }

    constexpr iterator& operator++() {
      ++value_;
      return *this;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      negation<incrementable<W>>
    >::value, int> = 0>
    constexpr void operator++(int) {
      ++value_;
    }
    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      incrementable<W>
    >::value, int> = 0>
    constexpr iterator operator++(int) {
      iterator tmp = *this;
      ++value_;
      return tmp;
    }


    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      detail::iv_decrementable<W>
    >::value, int> = 0>
    constexpr iterator& operator--() {
      --value_;
      return *this;
    }
    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      detail::iv_decrementable<W>
    >::value, int> = 0>
    constexpr iterator operator--(int) {
      iterator tmp = *this;
      --value_;
      return tmp;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      detail::iv_advanceable<W>,
      is_unsigned_integer_like<W>
    >::value, int> = 0>
    constexpr iterator& operator+=(difference_type n) {
      n < 0 ? (void)(value_ += static_cast<W>(n)) : (void)(value_ -= static_cast<W>(-n));
      return *this;
    }
    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      detail::iv_advanceable<W>,
      negation<is_unsigned_integer_like<W>>
    >::value, int> = 0>
    constexpr iterator& operator+=(difference_type n) {
      value_ += static_cast<W>(n);
      return *this;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      detail::iv_advanceable<W>,
      is_unsigned_integer_like<W>
    >::value, int> = 0>
    constexpr iterator& operator-=(difference_type n) {
      n < 0 ? (void)(value_ -= static_cast<W>(n)) : (void)(value_ += static_cast<W>(-n));
      return *this;
    }
    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      detail::iv_advanceable<W>,
      negation<is_unsigned_integer_like<W>>
    >::value, int> = 0>
    constexpr iterator& operator-=(difference_type n) {
      value_ -= n;
      return *this;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      detail::iv_advanceable<W>
    >::value, int> = 0>
    constexpr W operator[](difference_type n) const {
      return W(value_ + n);
    }

    friend constexpr std::enable_if_t<equality_comparable<W>::value, bool>
    operator==(const iterator& x, const iterator& y) {
      return x.value_ == y.value_;
    }
    friend constexpr std::enable_if_t<equality_comparable<W>::value, bool>
    operator!=(const iterator& x, const iterator& y) {
      return !(x == y);
    }
    friend constexpr std::enable_if_t<totally_ordered<W>::value, bool>
    operator<(const iterator& x, const iterator& y) {
      return x.value_ < y.value_;
    }
    friend constexpr std::enable_if_t<totally_ordered<W>::value, bool>
    operator>(const iterator& x, const iterator& y) {
      return y < x;
    }
    friend constexpr std::enable_if_t<totally_ordered<W>::value, bool>
    operator<=(const iterator& x, const iterator& y) {
      return !(y < x);
    }
    friend constexpr std::enable_if_t<totally_ordered<W>::value, bool>
    operator>=(const iterator& x, const iterator& y) {
      return !(x < y);
    }

    friend constexpr std::enable_if_t<detail::iv_advanceable<W>::value, iterator>
    operator+(iterator i, difference_type n) {
      i += n;
      return i;
    }
    friend constexpr std::enable_if_t<detail::iv_advanceable<W>::value, iterator>
    operator+(difference_type n, iterator i) {
      i += n;
      return i;
    }

    friend constexpr std::enable_if_t<detail::iv_advanceable<W>::value, iterator>
    operator-(iterator i, difference_type n) {
      i -= n;
      return i;
    }


    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      detail::iv_advanceable<W>,
      is_signed_integer_like<W>
    >::value, int> = 0>
    friend constexpr difference_type operator-(iterator x, iterator y) {
      return difference_type(difference_type(x.value_) - difference_type(y.value_));
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      detail::iv_advanceable<W>,
      negation<is_signed_integer_like<W>>,
      is_unsigned_integer_like<W>
    >::value, int> = 0>
    friend constexpr difference_type operator-(iterator x, iterator y) {
      using D = difference_type;
      return y.value_ > x.value_ ? D(-D(y.value_ - x.value_)) : D(x.value_ - y.value_);
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      detail::iv_advanceable<W>,
      negation<is_signed_integer_like<W>>,
      negation<is_unsigned_integer_like<W>>
    >::value, int> = 0>
    friend constexpr difference_type operator-(iterator x, iterator y) {
      return x.value_ - y.value_;
    }

   private:
    friend class iota_view;
    friend class sentinel;

    W value_;
  };

  class sentinel {
   public:
    constexpr sentinel() : bound_(Bound()) {}

    constexpr explicit sentinel(Bound bound) : bound_(bound) {}

    friend constexpr bool operator==(const iterator& x, const sentinel& y) {
      using namespace preview::rel_ops;
      return *x == y.bound_;
    }
    friend constexpr bool operator!=(const iterator& x, const sentinel& y) {
      return !(x == y);
    }
    friend constexpr bool operator==(const sentinel& y, const iterator& x) {
      return x == y;
    }
    friend constexpr bool operator!=(const sentinel& y, const iterator& x) {
      return !(x == y);
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      sized_sentinel_for<Bound, W>
    >::value, int> = 0>
    friend constexpr iter_difference_t<W> operator-(const iterator& x, const sentinel& y) {
      return *x - y.bound_;
    }

    template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      sized_sentinel_for<Bound, W>
    >::value, int> = 0>
    friend constexpr iter_difference_t<W> operator-(const sentinel& x, const iterator& y) {
      return -(*y - x.bound_);
    }

   private:
    friend class iota_view;

    Bound bound_;
  };


  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
    default_initializable<W>
  >::value, int> = 0>
  constexpr iota_view() : value_(W()), bound_(Bound()) {}

  constexpr explicit iota_view(W value) : value_(value), bound_(Bound()) {}

  constexpr explicit iota_view(type_identity_t<W> value,
                               type_identity_t<Bound> bound) : value_(value), bound_(bound) {}

  constexpr explicit iota_view(iterator first, typename detail::iv_ctor_iterator_last<W, Bound, iota_view>::type last)
      : value_(*first), bound_(get_value(last)) {}

  constexpr iterator begin() const {
    return iterator(value_);
  }


  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
    negation<same_as<W, Bound>>
  >::value, int> = 0>
  constexpr sentinel end() const {
    return sentinel(bound_);
  }
  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
    same_as<W, Bound>
  >::value, int> = 0>
  constexpr iterator end() const {
    return iterator(bound_);
  }

  constexpr bool empty() const {
    using namespace preview::rel_ops;
    return value_ == bound_;
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
    disjunction<
      conjunction<same_as<W, Bound>, detail::iv_advanceable<W>>,
      conjunction<is_integer_like<W>, is_integer_like<Bound>>,
      sized_sentinel_for<Bound, W>
    >
  >::value, int> = 0>
  constexpr auto size() const {
    return size_impl(conjunction<is_integer_like<W>, is_integer_like<Bound>>{});
  }

 private:
  template<typename T>
  static constexpr auto to_unsigned_like(T x) {
    using R = std::make_unsigned_t<T>;
    return static_cast<R>(x);
  }

  constexpr auto size_impl(std::true_type) const {
    return (value_ < 0)
        ? ((bound_ < 0)
            ? to_unsigned_like(-value_) - to_unsigned_like(-bound_)
            : to_unsigned_like(bound_) + to_unsigned_like(-value_)
          )
        : to_unsigned_like(bound_) - to_unsigned_like(value_);
  }
  constexpr auto size_impl(std::false_type) const {
    return static_cast<std::size_t>(bound_ - value_);
  }

  static constexpr W get_value(const iterator& i) {
    return i.value_;
  }
  static constexpr Bound get_value(const sentinel& s) {
    return s.bound_;
  }
  static constexpr unreachable_sentinel_t get_value(const unreachable_sentinel_t&) {
    return unreachable_sentinel_t{};
  }

  W value_ = W();
  Bound bound_ = Bound();
};

namespace views {
namespace detail {

struct iota_niebloid {
  template<typename W>
  constexpr iota_view<std::remove_reference_t<W>> operator()(W&& value) const {
    return ranges::iota_view<std::remove_reference_t<W>>(std::forward<W>(value));
  }

  template<typename W, typename Bound, std::enable_if_t<conjunction<
    disjunction<
      negation<is_integer_like<std::remove_reference_t<W>>>,
      negation<is_integer_like<std::remove_reference_t<Bound>>>,
      bool_constant<is_signed_integer_like<std::remove_reference_t<W>>::value == is_signed_integer_like<std::remove_reference_t<Bound>>::value>
    >
  >::value, int> = 0>
  constexpr ranges::iota_view<std::remove_reference_t<W>, std::remove_reference_t<Bound>>
  operator()(W&& value, Bound&& bound) const {
    return ranges::iota_view<std::remove_reference_t<W>, std::remove_reference_t<Bound>>(std::forward<W>(value), std::forward<Bound>(bound));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::iota_niebloid iota{};

} // namespace views

} // namespace ranges
} // namespace preview

// TODO: Implement constraints
template<typename W, typename Bound>
PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(preview::ranges::iota_view<W, Bound>) = true;

#endif // PREVIEW_RANGES_VIEWS_IOTA_H_
