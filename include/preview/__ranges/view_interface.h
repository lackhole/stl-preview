//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_VIEW_INTERFACE_H_
#define PREVIEW_RANGES_VIEW_INTERFACE_H_

#include <type_traits>

#include "preview/__memory/to_address.h"
#include "preview/__iterator/contiguous_iterator.h"
#include "preview/__iterator/prev.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__ranges/__forward_declare.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/bidirectional_range.h"
#include "preview/__ranges/cbegin.h"
#include "preview/__ranges/cend.h"
#include "preview/__ranges/common_range.h"
#include "preview/__ranges/empty.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/random_access_range.h"
#include "preview/__ranges/range_difference_t.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__ranges/sentinel_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename T, typename = void>
struct is_empty_callable : std::false_type {};

template<typename T>
struct is_empty_callable<T, void_t<decltype( ranges::empty(std::declval<T>()) )>> : std::true_type {};

template<typename T, bool = has_typename_type<iterator<T>>::value>
struct check_vi_data : std::false_type {};
template<typename T>
struct check_vi_data<T, true> : contiguous_iterator<iterator_t<T>> {};

template<
    typename T,
    bool =
      conjunction<
        forward_range<T>,
        has_typename_type<iterator<T>>,
        has_typename_type<sentinel<T>>
    >::value
>
struct check_vi_size : std::false_type {};

template<typename T>
struct check_vi_size<T, true> : sized_sentinel_for<sentinel_t<T>, iterator_t<T>> {};

} // namespace detail

template<typename Derived>
class view_interface {
 public:
  static_assert(std::is_class<Derived>::value && std::is_same<Derived, std::remove_cv_t<Derived>>::value,
                "Constraints not satisfied");


  using _$preview_derived = Derived;

  template<typename D = Derived, std::enable_if_t<
      sized_range<D>::value,
  int> = 0>
  constexpr bool empty() {
    using namespace preview::rel_ops;
    return ranges::size(derived()) == 0;
  }
  template<typename D = Derived, std::enable_if_t<conjunction<
      negation<sized_range<D>>,
      forward_range<D>
  >::value, int> = 0>
  constexpr bool empty() {
    using namespace preview::rel_ops;
    return ranges::begin(derived()) == ranges::end(derived());
  }

  template<typename D = Derived, std::enable_if_t<
      sized_range<const D>::value,
  int> = 0>
  constexpr bool empty() const {
    using namespace preview::rel_ops;
    return ranges::size(derived()) == 0;
  }
  template<typename D = Derived, std::enable_if_t<conjunction<
      negation<sized_range<const D>>,
      forward_range<const D>
  >::value, int> = 0>
  constexpr bool empty() const {
    using namespace preview::rel_ops;
    return ranges::begin(derived()) == ranges::end(derived());
  }

  constexpr auto cbegin() {
    return ranges::cbegin(derived());
  }

  template<typename D = Derived, std::enable_if_t<range<const D>::value, int> = 0>
  constexpr auto cbegin() const {
    return ranges::cbegin(derived());
  }

  constexpr auto cend() {
    return ranges::cend(derived());
  }

  template<typename D = Derived, std::enable_if_t<range<const D>::value, int> = 0>
  constexpr auto cend() const {
    return ranges::cend(derived());
  }

  template<typename D = Derived, std::enable_if_t<
    detail::is_empty_callable<D&>::value,
  int> = 0>
  constexpr explicit operator bool() {
    return !ranges::empty(derived());
  }

  template<typename D = Derived, std::enable_if_t<
    detail::is_empty_callable<const D&>::value,
  int> = 0>
  constexpr explicit operator bool() const {
    return !ranges::empty(derived());
  }

  template<typename D = Derived, std::enable_if_t<
      detail::check_vi_data<D>::value,
  int> = 0>
  constexpr auto data() {
    return preview::to_address(ranges::begin(derived()));
  }
  template<typename D = Derived, std::enable_if_t<
      detail::check_vi_data<const D>::value,
  int> = 0>
  constexpr auto data() const {
    return preview::to_address(ranges::begin(derived()));
  }

  template<typename D = Derived, std::enable_if_t<
      detail::check_vi_size<D>::value,
  int> = 0>
  constexpr auto size() {
    using T = decltype(ranges::end(derived()) - ranges::begin(derived()));
    return static_cast<T>(ranges::end(derived()) - ranges::begin(derived()));
  }
  template<typename D = Derived, std::enable_if_t<
      detail::check_vi_size<const D>::value,
  int> = 0>
  constexpr auto size() const {
    using T = decltype(ranges::end(derived()) - ranges::begin(derived()));
    return static_cast<T>(ranges::end(derived()) - ranges::begin(derived()));
  }

  template<typename D = Derived, std::enable_if_t<
      forward_range<D>::value,
  int> = 0>
  constexpr decltype(auto) front() {
    return *ranges::begin(derived());
  }
  template<typename D = Derived, std::enable_if_t<
      forward_range<const D>::value,
  int> = 0>
  constexpr decltype(auto) front() const {
    return *ranges::begin(derived());
  }

  template<typename D = Derived, std::enable_if_t<conjunction<
      bidirectional_range<D>,
      common_range<D>
  >::value, int> = 0>
  constexpr decltype(auto) back() {
    return *ranges::prev(ranges::end(derived()));
  }
  template<typename D = Derived, std::enable_if_t<conjunction<
      bidirectional_range<const D>,
      common_range<const D>
  >::value, int> = 0>
  constexpr decltype(auto) back() const {
    return *ranges::prev(ranges::end(derived()));
  }

  template<typename R = Derived, std::enable_if_t<
      random_access_range<R>::value,
  int> = 0>
  constexpr decltype(auto) operator[](range_difference_t<R> n) {
    return ranges::begin(derived())[n];
  }
  template<typename R = const Derived, std::enable_if_t<
      random_access_range<R>::value,
  int> = 0>
  constexpr decltype(auto) operator[](range_difference_t<R> n) const {
    return ranges::begin(derived())[n];
  }

 private:
  constexpr const Derived& derived() const {
    return static_cast<const Derived&>(*this);
  }

  constexpr Derived& derived() {
    return static_cast<Derived&>(*this);
  }
};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEW_INTERFACE_H_
