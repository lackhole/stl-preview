//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_VIEW_INTERFACE_H_
#define PREVIEW_RANGES_VIEW_INTERFACE_H_

#include <type_traits>

#include "preview/__concepts/different_from.h"
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
#include "preview/__ranges/size.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__ranges/sentinel_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/void_t.h"
#include "preview/__utility/cxx20_rel_ops.h"
#include "preview/__utility/to_unsigned_like.h"

namespace preview {
namespace ranges {
namespace detail {

// __GNUC__ < 11 defense
#if defined(__GNUC__) && (__GNUC__ < 11) && !defined(__clang__)
struct empty_fallback {
  constexpr empty_fallback& empty() const;
};
template<typename T>
struct test_empty_call : public T, empty_fallback {};

// ambiguous fallback. T have empty
template<typename T, typename = void>
struct derived_have_member_empty : std::true_type {};

// empty_fallback::empty() is callable. T doesn't have empty
template<typename T>
struct derived_have_member_empty<T, void_t<decltype( std::declval<test_empty_call<T>&>().empty() )>>
    : std::false_type {};

template<typename T, typename = void, typename = void>
struct comparable_range : std::false_type {};
template<typename T>
struct comparable_range<T, void_t<decltype( ranges::begin(std::declval<T&>()) )>, void_t<decltype( ranges::end(std::declval<T&>()) )>>
    : begin_end_comparable_range_impl<decltype( ranges::begin(std::declval<T&>()) ), decltype( ranges::end(std::declval<T&>()) )> {};

template<typename T>
struct is_empty_callable
    : disjunction<
        derived_have_member_empty<T>,
        is_invocable<decltype(ranges::size), T&>,
        comparable_range<T>
    > {};

#else
template<typename T>
struct is_empty_callable : is_invocable<decltype(ranges::empty), T&> {};
#endif

} // namespace detail

template<typename Derived>
class view_interface {
  template<typename D, typename...>
  struct derived_is_t;
  template<typename... B>
  struct derived_is_t<Derived, B...> : conjunction<B...> {};
  template<typename D, typename... B>
  using derived_is = std::enable_if_t<derived_is_t<D, B...>::value, int>;

 public:
  static_assert(std::is_class<Derived>::value, "Constraints not satisfied");
  static_assert(std::is_same<Derived, std::remove_cv_t<Derived>>::value, "Constraints not satisfied");

  using _$preview_derived = Derived;

  template<typename D = Derived, derived_is<D, disjunction<
      sized_range<D>,
      forward_range<D>
  >> = 0>
  constexpr bool empty() {
    return empty_impl(sized_range<D>{});
  }

  template<typename D = Derived, derived_is<D, disjunction<
      sized_range<const D>,
      forward_range<const D>
  >> = 0>
  constexpr bool empty() const {
    return empty_impl(sized_range<const D>{});
  }

  template<typename D = Derived, derived_is<D, input_range<D>> = 0>
  constexpr auto cbegin() {
    return ranges::cbegin(derived());
  }

  template<typename D = Derived, derived_is<D, input_range<const D>> = 0>
  constexpr auto cbegin() const {
    return ranges::cbegin(derived());
  }

  template<typename D = Derived, derived_is<D, input_range<D>> = 0>
  constexpr auto cend() {
    return ranges::cend(derived());
  }

  template<typename D = Derived, derived_is<D, input_range<const D>> = 0>
  constexpr auto cend() const {
    return ranges::cend(derived());
  }

  template<typename D = Derived, derived_is<D, detail::is_empty_callable<D>> = 0>
  constexpr explicit operator bool() {
    return !ranges::empty(derived());
  }

  template<typename D = Derived, derived_is<D, detail::is_empty_callable<const D>> = 0>
  constexpr explicit operator bool() const {
    return !ranges::empty(derived());
  }

  template<typename D = Derived, derived_is<D,
      has_typename_type<iterator<D>>,
      contiguous_iterator<iterator_t<D>>
  > = 0>
  constexpr auto data() {
    return preview::to_address(ranges::begin(derived()));
  }

  template<typename D = Derived, derived_is<D,
      range<const D>,
      contiguous_iterator<iterator_t<const D>>
  > = 0>
  constexpr auto data() const {
    return preview::to_address(ranges::begin(derived()));
  }

  template<typename D = Derived, derived_is<D,
      forward_range<D>,
      sized_sentinel_for<sentinel_t<D>, iterator_t<D>>
  > = 0>
  constexpr auto size() {
    return preview::to_unsigned_like(ranges::end(derived()) - ranges::begin(derived()));
  }

  template<typename D = Derived, derived_is<D,
      forward_range<const D>,
      sized_sentinel_for<sentinel_t<const D>, iterator_t<const D>>
  > = 0>
  constexpr auto size() const {
    return preview::to_unsigned_like(ranges::end(derived()) - ranges::begin(derived()));
  }

  template<typename D = Derived, derived_is<D, forward_range<D>> = 0>
  constexpr decltype(auto) front() {
    return *ranges::begin(derived());
  }

  template<typename D = Derived, derived_is<D, forward_range<const D>> = 0>
  constexpr decltype(auto) front() const {
    return *ranges::begin(derived());
  }

  template<typename D = Derived, derived_is<D,
      bidirectional_range<D>,
      common_range<D>
  > = 0>
  constexpr decltype(auto) back() {
    return *ranges::prev(ranges::end(derived()));
  }

  template<typename D = Derived, derived_is<D,
      bidirectional_range<const D>,
      common_range<const D>
  > = 0>
  constexpr decltype(auto) back() const {
    return *ranges::prev(ranges::end(derived()));
  }

  template<typename R = Derived, derived_is<R, random_access_range<R>> = 0>
  constexpr decltype(auto) operator[](range_difference_t<R> n) {
    return ranges::begin(derived())[n];
  }

  template<typename R = Derived, derived_is<R, random_access_range<const R>> = 0>
  constexpr decltype(auto) operator[](range_difference_t<const R> n) const {
    return ranges::begin(derived())[n];
  }

 private:
  constexpr bool empty_impl(std::true_type /* sized_range<Derived> */) {
    using namespace preview::rel_ops;
    return ranges::size(derived()) == 0;
  }
  constexpr bool empty_impl(std::false_type /* sized_range<Derived> */) {
    using namespace preview::rel_ops;
    return ranges::begin(derived()) == ranges::end(derived());
  }
  constexpr bool empty_impl(std::true_type /* sized_range<Derived> */) const {
    using namespace preview::rel_ops;
    return ranges::size(derived()) == 0;
  }
  constexpr bool empty_impl(std::false_type /* sized_range<Derived> */) const {
    using namespace preview::rel_ops;
    return ranges::begin(derived()) == ranges::end(derived());
  }

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
