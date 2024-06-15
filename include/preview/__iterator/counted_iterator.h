//
// Created by cosge on 2024-01-03.
//

#ifndef PREVIEW_ITERATOR_COUNTED_ITERATOR_H_
#define PREVIEW_ITERATOR_COUNTED_ITERATOR_H_

#include <iterator>
#include <type_traits>
#include <utility>

#include "preview/__core/constexpr.h"
#include "preview/__concepts/assignable_from.h"
#include "preview/__concepts/common_with.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__concepts/dereferenceable.h"
#include "preview/__iterator/detail/std_check.h"
#include "preview/__iterator/contiguous_iterator.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/bidirectional_iterator.h"
#include "preview/__iterator/default_sentinel_t.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/indirectly_readable.h"
#include "preview/__iterator/indirectly_swappable.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/input_or_output_iterator.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/iter_move.h"
#include "preview/__iterator/iter_rvalue_reference_t.h"
#include "preview/__iterator/iter_swap.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__iterator/random_access_iterator.h"
#include "preview/__memory/to_address.h"
#include "preview/__type_traits/void_t.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace detail {

template<typename I, bool = indirectly_readable<I>::value /* true */>
struct counted_iterator_value_type {
  using value_type = iter_value_t<I>;
};
template<typename I>
struct counted_iterator_value_type<I, false> {};

template<typename I, typename = void>
struct counted_iterator_iterator_concept {};
template<typename I>
struct counted_iterator_iterator_concept<I, void_t<typename I::iterator_concept>> {
  using iterator_concept = typename I::iterator_concept;
};

template<typename I, typename = void>
struct counted_iterator_iterator_category {};
template<typename I>
struct counted_iterator_iterator_category<I, void_t<typename I::iterator_category>> {
  using iterator_category = typename I::iterator_category;
};

} // namespace detail

template<typename I>
class counted_iterator
    : public detail::counted_iterator_value_type<I>
    , public detail::counted_iterator_iterator_concept<I>
    , public detail::counted_iterator_iterator_category<I>
{
 public:
  static_assert(input_or_output_iterator<I>::value, "Constraints not satisfied");

  using iterator_type = I;
  using difference_type = iter_difference_t<I>;

  constexpr counted_iterator() = default;

  constexpr counted_iterator(I x, iter_difference_t<I> n)
      : current_(std::move(x)), length_(n) {}

  template<typename I2, std::enable_if_t<convertible_to<const I2&, I>::value, int> = 0>
  constexpr counted_iterator(const counted_iterator<I2>& other)
      : current_(other.current_), length_(other.length_) {}


  template<typename I2, std::enable_if_t<assignable_from<I&, const I2&>::value, int> = 0>
  constexpr counted_iterator& operator=(const counted_iterator<I2>& other) {
    current_ = other.current_;
    length_ = other.length_;
    return *this;
  }


  constexpr const I& base() const& noexcept {
    return current_;
  }
  constexpr I base() && {
    return std::move(current_);
  }


  constexpr difference_type count() const noexcept {
    return length_;
  }


  constexpr decltype(auto) operator*() {
    return *current_;
  }
  template<typename I2 = I, std::enable_if_t<dereferenceable<const I2>::value, int> = 0>
  constexpr decltype(auto) operator*() const {
    return *current_;
  }
  template<typename I2 = I, std::enable_if_t<contiguous_iterator<I2>::value, int> = 0>
  constexpr auto operator->() const noexcept {
    return preview::to_address(current_);
  }


  template<typename I2 = I, std::enable_if_t<random_access_iterator<I2>::value, int> = 0>
  constexpr decltype(auto) operator[](iter_difference_t<I> n) const {
    return this->base()[n];
  }


  constexpr counted_iterator& operator++() {
    ++current_;
    --length_;
    return *this;

  }
  template<typename I2 = I, std::enable_if_t<forward_iterator<I2>::value == false, int> = 0>
  PREVIEW_CONSTEXPR_AFTER_CXX20 decltype(auto) operator++(int) {
    --length_;
    try {
      return current_++;
    } catch (...) {
      ++length_;
      throw;
    }
  }
  template<typename I2 = I, std::enable_if_t<forward_iterator<I2>::value, int> = 0>
  constexpr counted_iterator operator++(int) {
    counted_iterator temp{*this};
    ++*this;
    return temp;
  }
  template<typename I2 = I, std::enable_if_t<bidirectional_iterator<I2>::value, int> = 0>
  constexpr counted_iterator& operator--() {
    --current_;
    ++length_;
    return *this;
  }
  template<typename I2 = I, std::enable_if_t<bidirectional_iterator<I2>::value, int> = 0>
  constexpr counted_iterator operator--(int) {
    counted_iterator temp{*this};
    --*this;
    return temp;
  }

  template<typename I2 = I, std::enable_if_t<random_access_iterator<I2>::value, int> = 0>
  constexpr counted_iterator operator+(iter_difference_t<I> n) const {
    return counted_iterator(current_ + n, length_ - n);
  }
  template<typename I2 = I, std::enable_if_t<random_access_iterator<I2>::value, int> = 0>
  constexpr counted_iterator& operator+=(iter_difference_t<I> n) {
    current_ += n;
    length_ -= n;
    return *this;
  }

  template<typename I2 = I, std::enable_if_t<random_access_iterator<I2>::value, int> = 0>
  constexpr counted_iterator operator-(iter_difference_t<I> n) const {
    return counted_iterator(current_ - n, length_ + n);
  }
  template<typename I2 = I, std::enable_if_t<random_access_iterator<I2>::value, int> = 0>
  constexpr counted_iterator& operator-=(iter_difference_t<I> n) {
    current_ -= n;
    length_ += n;
    return *this;
  }

  template<typename I2, std::enable_if_t<common_with<I, I2>::value, int> = 0>
  friend constexpr bool operator==(const counted_iterator& x, const counted_iterator<I2>& y) {
    using namespace preview::rel_ops;
    return x.count() == y.count();
  }

  template<typename I2, std::enable_if_t<common_with<I, I2>::value, int> = 0>
  friend constexpr bool operator<(const counted_iterator& x, const counted_iterator<I2>& y) {
    using namespace preview::rel_ops;
    return y.count() < x.count();
  }

  template<typename I2, std::enable_if_t<common_with<I, I2>::value, int> = 0>
  friend constexpr bool operator!=(const counted_iterator& x, const counted_iterator<I2>& y) {
    using namespace preview::rel_ops;
    return x.count() != y.count();
  }
  template<typename I2, std::enable_if_t<common_with<I, I2>::value, int> = 0>
  friend constexpr bool operator<=(const counted_iterator& x, const counted_iterator<I2>& y) {
    using namespace preview::rel_ops;
    return y.count() <= x.count();
  }
  template<typename I2, std::enable_if_t<common_with<I, I2>::value, int> = 0>
  friend constexpr bool operator>(const counted_iterator& x, const counted_iterator<I2>& y) {
    using namespace preview::rel_ops;
    return y.count() > x.count();
  }
  template<typename I2, std::enable_if_t<common_with<I, I2>::value, int> = 0>
  friend constexpr bool operator>=(const counted_iterator& x, const counted_iterator<I2>& y) {
    using namespace preview::rel_ops;
    return y.count() >= x.count();
  }


  friend constexpr bool operator==(const counted_iterator& x, default_sentinel_t) {
    return x.count() == 0;
  }
  friend constexpr bool operator==(default_sentinel_t, const counted_iterator& x) {
    return x.count() == 0;
  }
  friend constexpr bool operator!=(const counted_iterator& x, default_sentinel_t) {
    return x.count() != 0;
  }
  friend constexpr bool operator!=(default_sentinel_t, const counted_iterator& x) {
    return x.count() != 0;
  }


  template<typename I2 = I>
  friend constexpr std::enable_if_t<random_access_iterator<I2>::value, counted_iterator>
  operator+(iter_difference_t<I> n, const counted_iterator& x) {
    return x + n;
  }

  template<typename I2>
  friend constexpr iter_difference_t<I2>
  operator-(const counted_iterator& x, const counted_iterator<I2>& y) {
    return y.count() - x.count();
  }

  friend constexpr iter_difference_t<I>
  operator-(const counted_iterator& x, default_sentinel_t) {
    return -x.count();
  }

  friend constexpr iter_difference_t<I>
  operator-(default_sentinel_t, const counted_iterator& y) {
    return y.count();
  }

  template<typename I2 = I, std::enable_if_t<input_iterator<I2>::value, int> = 0>
  friend constexpr iter_rvalue_reference_t<I2>
  iter_move(const counted_iterator& i) noexcept(noexcept(ranges::iter_move(i.base()))) {
    return ranges::iter_move(i.base());
  }

  template<typename I2, std::enable_if_t<indirectly_swappable<I2, I>::value, int> = 0>
  friend constexpr void iter_swap(const counted_iterator& x, const counted_iterator<I2>& y)
      noexcept(noexcept(ranges::iter_swap(x.base(), y.base())))
  {
    ranges::iter_swap(x.base(), y.base());
  }

 private:
  iterator_type current_;
  difference_type length_ = 0;
};

namespace detail {

template<typename I, bool = conjunction<
    input_iterator<I>,
    is_specialized_iterator_traits<iterator_traits<I>>
>::value /* true */>
struct counted_iterator_iterator_traits : iterator_traits_sfinae<counted_iterator<I>> {};

template<typename I>
struct counted_iterator_iterator_traits<I, false> : iterator_traits<I> {
  using pointer = std::conditional_t<
      contiguous_iterator<I>::value,
      std::add_pointer_t<iter_reference_t<I>>,
      void
  >;
};

template<typename I>
struct is_specialized_iterator_traits<std::iterator_traits<counted_iterator<I>>>
    : conjunction<
        input_iterator<I>,
        is_specialized_iterator_traits<iterator_traits<I>>
    > {};

template<typename I>
struct is_specialized_iterator_traits<iterator_traits<counted_iterator<I>>>
    : conjunction<
        input_iterator<I>,
        is_specialized_iterator_traits<iterator_traits<I>>
    > {};

} // namespace detail

// specialization
template<typename I>
struct iterator_traits<counted_iterator<I>>
    : detail::counted_iterator_iterator_traits<I> {};

} // namespace preview

#if PREVIEW_STD_HAVE_CXX20_ITERATOR

namespace std {

template<std::input_iterator I>
requires(preview::detail::is_primary_iterator_traits<I>::value == false)
struct iterator_traits<preview::counted_iterator<I>> : iterator_traits<I> {
  using pointer = typename preview::iterator_traits<preview::counted_iterator<I>>::pointer;
};

} // namespace std

#endif

#endif // PREVIEW_ITERATOR_COUNTED_ITERATOR_H_
