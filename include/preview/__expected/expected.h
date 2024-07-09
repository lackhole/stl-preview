//
// Created by YongGyu Lee on 3/14/24.
//

#ifndef PREVIEW_EXPECTED_EXPECTED_H_
#define PREVIEW_EXPECTED_EXPECTED_H_

#include <new>
#include <type_traits>
#include <utility>

#include "preview/__concepts/convertible_to.h"
#include "preview/__concepts/copy_constructible.h"
#include "preview/__concepts/move_constructible.h"
#include "preview/__core/constexpr.h"
#include "preview/__core/nodiscard.h"
#include "preview/__expected/bad_expected_access.h"
#include "preview/__expected/monadic.h"
#include "preview/__expected/unexpected.h"
#include "preview/__expected/unexpect.h"
#include "preview/__functional/invoke.h"
#include "preview/__memory/addressof.h"
#include "preview/__memory/construct_at.h"
#include "preview/__memory/destroy_at.h"
#include "preview/__type_traits/has_typename_value_type.h"
#include "preview/__type_traits/is_specialization.h"
#include "preview/__type_traits/is_swappable.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__utility/as_const.h"
#include "preview/__utility/cxx20_rel_ops.h"
#include "preview/__utility/in_place.h"
#include "preview/variant.h"

namespace preview {

template<typename T, typename E>
class expected;

namespace detail {

struct void_placeholder_t {};

template<typename T>
using void_placdholder_or_t = std::conditional_t<std::is_void<T>::value, void_placeholder_t, T>;

template<typename NewType, typename OldType, typename... Args>
PREVIEW_CONSTEXPR_AFTER_CXX20
void reinit_expected_impl(std::false_type, std::false_type,
                          NewType& new_val, OldType& old_val, Args&&... args) {
  OldType temp(std::move(old_val));
  preview::destroy_at(preview::addressof(old_val));
  try {
    preview::construct_at(preview::addressof(new_val), std::forward<Args>(args)...);
  } catch (...) {
    preview::construct_at(preview::addressof(old_val), std::move(temp));
    throw;
  }
}

template<typename NewType, typename OldType, typename... Args>
constexpr void reinit_expected_impl(std::false_type, std::true_type,
                                    NewType& new_val, OldType& old_val, Args&&... args) {
  NewType temp(std::forward<Args>(args)...);
  preview::destroy_at(preview::addressof(old_val));
  preview::construct_at(preview::addressof(new_val), std::move(temp));
}

template<typename Any, typename NewType, typename OldType, typename... Args>
constexpr void reinit_expected_impl(std::true_type, Any,
                                    NewType& new_val, OldType& old_val, Args&&... args) {
  preview::destroy_at(preview::addressof(old_val));
  preview::construct_at(preview::addressof(new_val), std::forward<Args>(args)...);
}

template<typename NewType, typename OldType, typename... Args>
PREVIEW_CONSTEXPR_AFTER_CXX20
void reinit_expected(NewType& new_val, OldType& old_val, Args&&... args) {
  reinit_expected_impl(
      std::is_nothrow_constructible<NewType, Args...>{},
      std::is_nothrow_move_constructible<NewType>{},
      new_val, old_val, std::forward<Args>(args)...);
}

template<bool IsTriviallyDestructible, typename T, typename E>
struct expected_storage_t {};

template<typename T, typename E>
using expected_storage = expected_storage_t<
    conjunction<std::is_trivially_destructible<T>, std::is_trivially_destructible<E>>::value, T, E>;

template<typename T, typename E>
struct expected_storage_t<true, T, E> {
  // Resolution handled in expected
  constexpr expected_storage_t() {}

  constexpr explicit expected_storage_t(bool has_value)
      : has_value_(has_value) {}

  constexpr explicit expected_storage_t(in_place_index_t<0>, void_placeholder_t) noexcept
      : has_value_(true) {}

  template<typename... Args>
  constexpr explicit expected_storage_t(in_place_index_t<0>, Args&&... args)
  noexcept(std::is_nothrow_constructible<T, Args...>::value)
      : has_value_(true)
      , value_(std::forward<Args>(args)...) {}

  template<typename U, typename... Args>
  constexpr explicit expected_storage_t(in_place_index_t<0>, std::initializer_list<U> il, Args&&... args)
  noexcept(std::is_nothrow_constructible<T, std::initializer_list<U>&, Args...>::value)
      : has_value_(true)
      , value_(il, std::forward<Args>(args)...) {}

  template<typename... Args>
  constexpr explicit expected_storage_t(in_place_index_t<1>, Args&&... args)
  noexcept(std::is_nothrow_constructible<T, Args...>::value)
      : has_value_(false)
      , error_(std::forward<Args>(args)...) {}

  template<typename U, typename... Args>
  constexpr explicit expected_storage_t(in_place_index_t<1>, std::initializer_list<U> il, Args&&... args)
  noexcept(std::is_nothrow_constructible<T, std::initializer_list<U>&, Args...>::value)
      : has_value_(false)
      , error_(il, std::forward<Args>(args)...) {}

  bool has_value_;
  union {
    T value_;
    E error_;
  };
};

template<typename T, typename E>
struct expected_storage_t<false, T, E> {
  // Resolution handled in expected
  constexpr expected_storage_t() {}

  constexpr explicit expected_storage_t(bool has_value)
      : has_value_(has_value) {}

  template<typename... Args>
  constexpr explicit expected_storage_t(in_place_index_t<0>, Args&&... args)
      noexcept(std::is_nothrow_constructible<T, Args...>::value)
      : has_value_(true)
      , value_(std::forward<Args>(args)...) {}

  template<typename U, typename... Args>
  constexpr explicit expected_storage_t(in_place_index_t<0>, std::initializer_list<U> il, Args&&... args)
      noexcept(std::is_nothrow_constructible<T, std::initializer_list<U>&, Args...>::value)
      : has_value_(true)
      , value_(il, std::forward<Args>(args)...) {}

  template<typename... Args>
  constexpr explicit expected_storage_t(in_place_index_t<1>, Args&&... args)
      noexcept(std::is_nothrow_constructible<T, Args...>::value)
      : has_value_(false)
      , error_(std::forward<Args>(args)...) {}

  template<typename U, typename... Args>
  constexpr explicit expected_storage_t(in_place_index_t<1>, std::initializer_list<U> il, Args&&... args)
      noexcept(std::is_nothrow_constructible<T, std::initializer_list<U>&, Args...>::value)
      : has_value_(false)
      , error_(il, std::forward<Args>(args)...) {}

  PREVIEW_CONSTEXPR_AFTER_CXX20 ~expected_storage_t() noexcept {}

  constexpr expected_storage_t(const expected_storage_t&) = default;
  constexpr expected_storage_t(expected_storage_t&&) = default;
  constexpr expected_storage_t& operator=(const expected_storage_t&) = default;
  constexpr expected_storage_t& operator=(expected_storage_t&&) = default;

  bool has_value_;
  union {
    T value_;
    E error_;
  };
};

// T is never void. void_placeholder_t is used instead.
template<typename T, typename E>
struct expected_base {
  static_assert(!std::is_void<T>::value, "T should be void_placeholder_t");

  // Resolution handled in expected
  constexpr expected_base() = default;

  constexpr explicit expected_base(bool has_value)
      : storage(has_value) {}

  template<std::size_t I, typename... Args>
  constexpr explicit expected_base(in_place_index_t<I>, Args&&... args)
      : storage(in_place_index<I>, std::forward<Args>(args)...) {}

  template<std::size_t I, typename U, typename... Args>
  constexpr explicit expected_base(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
      : storage(in_place_index<I>, il, std::forward<Args>(args)...) {}

  constexpr void construct_from(const expected_base& other)
      noexcept(conjunction<
          std::is_nothrow_copy_constructible<T>,
          std::is_nothrow_copy_constructible<E>
      >::value)
  {
    storage.has_value_ = other.has_value();
    if (other.has_value()) {
      construct_value_from(other);
    } else {
      construct_error_from(other);
    }
  }

  constexpr void construct_from(expected_base&& other)
      noexcept(conjunction<
          std::is_nothrow_move_constructible<T>,
          std::is_nothrow_move_constructible<E>
      >::value)
  {
    storage.has_value_ = other.has_value();
    if (other.has_value()) {
      construct_value_from(std::move(other));
    } else {
      construct_error_from(std::move(other));
    }
  }

  constexpr void assign_from(const expected_base& other) {
    if (has_value() == other.has_value()) {
      if (has_value()) {
        assign_value_from(other);
      } else {
        storage.error_ = other.error();
      }
    } else {
      if (has_value()) {
        reinit_error_and_destroy_value(other.error());
      } else {
        reinit_value_and_destroy_error_from(other);
      }
      storage.has_value_ = other.has_value();
    }
  }

  constexpr void assign_from(expected_base&& other) {
    if (has_value() == other.has_value()) {
      if (has_value())
        assign_value_from(std::move(other));
      else
        storage.error_ = std::move(other).error();
    } else {
      if (has_value()) {
        reinit_error_and_destroy_value(std::move(other.error()));
        storage.has_value_ = false;
      } else { // other.has_value()
        reinit_value_and_destroy_error_from(std::move(other));
        storage.has_value_ = true;
      }
    }
  }

  template<typename U = T, std::enable_if_t<std::is_same<U, void_placeholder_t>::value, int> = 0>
  constexpr void reinit_value_and_destroy_error() {
    destroy_error();
  }

  template<typename U = T, typename... Args, std::enable_if_t<!std::is_same<U, void_placeholder_t>::value, int> = 0>
  PREVIEW_CONSTEXPR_AFTER_CXX20 void reinit_value_and_destroy_error(Args&&... args) {
    reinit_expected(value(), error(), std::forward<Args>(args)...);
  }

  template<typename U = T, typename Other, std::enable_if_t<std::is_same<U, void_placeholder_t>::value, int> = 0>
  constexpr void reinit_value_and_destroy_error_from(Other&&) {
    destroy_error();
  }

  template<typename U = T, typename Other, std::enable_if_t<!std::is_same<U, void_placeholder_t>::value, int> = 0>
  PREVIEW_CONSTEXPR_AFTER_CXX20 void reinit_value_and_destroy_error_from(Other&& other) {
    reinit_value_and_destroy_error(std::forward<Other>(other).value());
  }

  template<typename U = T, typename... Args, std::enable_if_t<std::is_same<U, void_placeholder_t>::value, int> = 0>
  constexpr void reinit_error_and_destroy_value(Args&&... args) {
    construct_error(std::forward<Args>(args)...);
  }

  template<typename U = T, typename... Args, std::enable_if_t<!std::is_same<U, void_placeholder_t>::value, int> = 0>
  PREVIEW_CONSTEXPR_AFTER_CXX20 void reinit_error_and_destroy_value(Args&&... args) {
    reinit_expected(error(), value(), std::forward<Args>(args)...);
  }

  template<typename U = T, typename... Args, std::enable_if_t<std::is_same<U, void_placeholder_t>::value, int> = 0>
  constexpr void construct_value(Args&&...) noexcept {}

  template<typename U = T, typename... Args, std::enable_if_t<!std::is_same<U, void_placeholder_t>::value, int> = 0>
  constexpr void construct_value(Args&&... args) {
    preview::construct_at(preview::addressof(storage.value_), std::forward<Args>(args)...);
  }

  template<typename U = T, typename Other, std::enable_if_t<std::is_same<U, void_placeholder_t>::value, int> = 0>
  constexpr void construct_value_from(Other&&) noexcept {}

  template<typename U = T, typename Other, std::enable_if_t<!std::is_same<U, void_placeholder_t>::value, int> = 0>
  constexpr void construct_value_from(Other&& other)
  noexcept(std::is_nothrow_constructible<T, decltype(std::forward<Other>(other).value())>::value) {
    construct_value(std::forward<Other>(other).value());
  }

  template<typename U = T, typename Other, std::enable_if_t<std::is_same<U, void_placeholder_t>::value, int> = 0>
  constexpr void assign_value_from(Other&&) noexcept {}

  template<typename U = T, typename Other, std::enable_if_t<!std::is_same<U, void_placeholder_t>::value, int> = 0>
  constexpr void assign_value_from(Other&& other) {
    storage.value_ = std::forward<Other>(other).value();
  }

  template<typename G>
  constexpr void construct_error(G&& e) {
    preview::construct_at(preview::addressof(storage.error_), std::forward<G>(e));
  }

  template<typename Other>
  constexpr void construct_error_from(Other&& other) {
    construct_error(std::forward<Other>(other).error());
  }

  PREVIEW_CONSTEXPR_AFTER_CXX20 void destroy() {
    has_value() ? destroy_value() : destroy_error();
  }

  template<typename U = T, std::enable_if_t<std::is_trivially_destructible<U>::value, int> = 0>
  constexpr void destroy_value() {}
  template<typename U = T, std::enable_if_t<!std::is_trivially_destructible<U>::value, int> = 0>
  PREVIEW_CONSTEXPR_AFTER_CXX20 void destroy_value() {
    value().~T();
  }

  template<typename U = E, std::enable_if_t<std::is_trivially_destructible<U>::value, int> = 0>
  constexpr void destroy_error() {}
  template<typename U = E, std::enable_if_t<!std::is_trivially_destructible<U>::value, int> = 0>
  PREVIEW_CONSTEXPR_AFTER_CXX20 void destroy_error() {
    error().~E();
  }

  PREVIEW_NODISCARD constexpr bool has_value() const noexcept {
    return storage.has_value_;
  }

  constexpr void has_value(bool new_value) noexcept {
    storage.has_value_ = new_value;
  }

  constexpr T& value() & noexcept { return storage.value_; }
  constexpr const T& value() const& noexcept { return storage.value_; }
  constexpr T&& value() && noexcept { return std::move(storage.value_); }
  constexpr const T&& value() const&& noexcept { return std::move(storage.value_); }

  constexpr E& error() & noexcept { return storage.error_; }
  constexpr const E& error() const& noexcept { return storage.error_; }
  constexpr E&& error() && noexcept { return std::move(storage.error_); }
  constexpr const E&& error() const&& noexcept { return std::move(storage.error_); }

  expected_storage<T, E> storage;
};

template<typename T, typename E>
struct expected_nontrivial_dtor : expected_base<T, E> {
  using base = expected_base<T, E>;
  using base::base;

  PREVIEW_CONSTEXPR_AFTER_CXX20 ~expected_nontrivial_dtor() {
    base::destroy();
  }

  expected_nontrivial_dtor() = default;
  expected_nontrivial_dtor(const expected_nontrivial_dtor&) = default;
  expected_nontrivial_dtor(expected_nontrivial_dtor&&) = default;
  expected_nontrivial_dtor& operator=(const expected_nontrivial_dtor&) = default;
  expected_nontrivial_dtor& operator=(expected_nontrivial_dtor&&) = default;
};

template<typename T, typename E>
using expected_control_smf =
    std::conditional_t<
        conjunction<std::is_trivially_destructible<T>, std::is_trivially_destructible<E>>::value,
        control_special<expected_base<T, E>, T, E>,
        control_special<expected_nontrivial_dtor<T, E>, T, E>
    >;



} // namespace detail

template<typename T, typename E>
class expected : private detail::expected_control_smf<detail::void_placdholder_or_t<T>, E> {
  using base = detail::expected_control_smf<detail::void_placdholder_or_t<T>, E>;

  // monadic functors
  using and_then_t = detail::expected_and_then_t<expected>;
  using transform_t = detail::expected_transform_t<expected>;
  using or_else_t = detail::expected_or_else_t<expected>;
  using transform_error_t = detail::expected_transform_error_t<expected>;

  // swap exception
  using value_nothrow_swappable = conjunction<
      detail::void_or<T, std::is_nothrow_move_constructible, T>,
      detail::void_or<T, is_nothrow_swappable, T>>;
  using error_nothrow_swappable = conjunction<
      std::is_nothrow_move_constructible<E>,
      is_nothrow_swappable<E>>;
  using nothrow_swappable = conjunction<value_nothrow_swappable, error_nothrow_swappable>;

 public:
  static_assert(!std::is_reference<T>::value, "Constraints not satisfied");
  static_assert(!std::is_array<T>::value, "Constraints not satisfied");
  static_assert(!std::is_function<T>::value, "Constraints not satisfied");
  static_assert(!is_specialization<T, unexpected>::value, "Constraints not satisfied");
  static_assert(!std::is_same<T, in_place_t>::value, "Constraints not satisfied");
  static_assert(!std::is_same<T, unexpect_t>::value, "Constraints not satisfied");

  using value_type = T;
  using error_type = E;
  using unexpected_type = unexpected<E>;

  template<typename U>
  using rebind = expected<U, E>;

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      detail::void_or<T, std::is_default_constructible, T>
  >::value, int> = 0>
  constexpr expected() : base(in_place_index<0>) {}

  constexpr expected(const expected& other) = default;
  constexpr expected(expected&& other) = default;

  template<typename U, typename G, std::enable_if_t<conjunction<
      negation<conjunction< /* explicit */
          std::is_convertible<std::add_lvalue_reference_t<const U>, T>,
          std::is_convertible<const G&, E>>>,
      disjunction<
          conjunction<std::is_void<T>, std::is_void<U>>,
          std::is_constructible<T, std::add_lvalue_reference_t<const U>>
      >,
      std::is_constructible<E, const G&>
  >::value, int> = 0>
  constexpr explicit expected(const expected<U, G>& other)
      : base(other.has_value())
  {
    if (other.has_value()) {
      base::construct_value_from(other);
    } else {
      base::construct_error_from(other);
    }
  }

  template<typename U, typename G, std::enable_if_t<conjunction<
      conjunction< /* explicit */
          std::is_convertible<std::add_lvalue_reference_t<const U>, T>,
          std::is_convertible<const G&, E>>,
      disjunction<
          conjunction<std::is_void<T>, std::is_void<U>>,
          std::is_constructible<T, std::add_lvalue_reference_t<const U>>
      >,
      std::is_constructible<E, const G&>
  >::value, int> = 0>
  constexpr expected(const expected<U, G>& other)
      : base(other.has_value())
  {
    if (other.has_value()) {
      base::construct_value_from(other);
    } else {
      base::construct_error_from(other);
    }
  }

  template<typename U, typename G, std::enable_if_t<conjunction<
      negation<conjunction< /* explicit */
          std::is_convertible<U, T>,
          std::is_convertible<G, E>>>,
      disjunction<
          conjunction<std::is_void<T>, std::is_void<U>>,
          std::is_constructible<T, U>
      >,
      std::is_constructible<E, G>
  >::value, int> = 0>
  constexpr explicit expected(expected<U, G>&& other)
      : base(other.has_value())
  {
    if (other.has_value()) {
      base::construct_value_from(std::move(other));
    } else {
      base::construct_error_from(std::move(other));
    }
  }

  template<typename U, typename G, std::enable_if_t<conjunction<
      conjunction< /* explicit */
          std::is_convertible<U, T>,
          std::is_convertible<G, E>>,
      disjunction<
          conjunction<std::is_void<T>, std::is_void<U>>,
          std::is_constructible<T, U>
      >,
      std::is_constructible<E, G>
  >::value, int> = 0>
  constexpr expected(expected<U, G>&& other)
      : base(other.has_value())
  {
    if (other.has_value()) {
      base::construct_value_from(std::move(other));
    } else {
      base::construct_error_from(std::move(other));
    }
  }

  template<typename U = T, std::enable_if_t<conjunction<
      negation<std::is_void<T>>,
      negation<std::is_same<remove_cvref_t<U>, in_place_t>>,
      negation<std::is_same<expected, remove_cvref_t<U>>>,
      std::is_constructible<T, U>,
      negation<is_specialization<remove_cvref_t<U>, unexpected>>,
      disjunction<
          negation<std::is_same<bool, std::remove_cv_t<T>>>,
          negation<is_specialization<remove_cvref_t<U>, expected>>
      >,
      negation<std::is_convertible<U, T>>
  >::value, int> = 0>
  constexpr explicit expected(U&& v)
      : base(in_place_index<0>, std::forward<U>(v)) {}

  template<typename U = T, std::enable_if_t<conjunction<
      negation<std::is_void<T>>,
      negation<std::is_same<remove_cvref_t<U>, in_place_t>>,
      negation<std::is_same<expected, remove_cvref_t<U>>>,
      std::is_constructible<T, U>,
      negation<is_specialization<remove_cvref_t<U>, unexpected>>,
      disjunction<
          negation<std::is_same<bool, std::remove_cv_t<T>>>,
          negation<is_specialization<remove_cvref_t<U>, expected>>
      >,
      std::is_convertible<U, T>
  >::value, int> = 0>
  constexpr expected(U&& v)
      : base(in_place_index<0>, std::forward<U>(v)) {}

  template<typename G, std::enable_if_t<conjunction<
      std::is_constructible<E, const G&>,
      negation<std::is_convertible<const G&, E>>
  >::value, int> = 0>
  constexpr explicit expected(const unexpected<G>& e)
      : base(in_place_index<1>, e.error()) {}

  template<typename G, std::enable_if_t<conjunction<
      std::is_constructible<E, G>,
      std::is_convertible<const G&, E>
  >::value, int> = 0>
  constexpr expected(const unexpected<G>& e)
      : base(in_place_index<1>, e.error()) {}

  template<typename G, std::enable_if_t<conjunction<
      std::is_constructible<E, G>,
      negation<std::is_convertible<G, E>>
  >::value, int> = 0>
  constexpr explicit expected(unexpected<G>&& e)
      : base(in_place_index<1>, std::move(e.error())) {}

  template<typename G, std::enable_if_t<conjunction<
      std::is_constructible<E, G>,
      std::is_convertible<G, E>
  >::value, int> = 0>
  constexpr expected(unexpected<G>&& e)
      : base(in_place_index<1>, std::move(e.error())) {}

  template<typename... Args, std::enable_if_t<conjunction<
      negation<std::is_void<T>>,
      std::is_constructible<T, Args...>
  >::value, int> = 0>
  constexpr explicit expected(in_place_t, Args&&... args)
      : base(in_place_index<0>, std::forward<Args>(args)...) {}

  template<typename U, typename... Args, std::enable_if_t<conjunction<
      negation<std::is_void<T>>,
      std::is_constructible<T, std::initializer_list<U>&, Args...>
  >::value, int> = 0>
  constexpr explicit expected(in_place_t, std::initializer_list<U> il, Args&&... args)
      : base(in_place_index<0>, il, std::forward<Args>(args)...) {}

  template<typename U = T, std::enable_if_t<std::is_void<U>::value, int> = 0>
  constexpr explicit expected(in_place_t) noexcept
      : base(in_place_index<0>) {}

  template<typename... Args, std::enable_if_t<
      std::is_constructible<E, Args...>
  ::value, int> = 0>
  constexpr explicit expected(unexpect_t, Args&&... args)
      : base(in_place_index<1>, std::forward<Args>(args)...) {}

  template<typename U, typename... Args, std::enable_if_t<
      std::is_constructible<E, std::initializer_list<U>&, Args...>
  ::value, int> = 0>
  constexpr explicit expected(unexpect_t, std::initializer_list<U> il, Args&&... args)
      : base(in_place_index<1>, il, std::forward<Args>(args)...) {}

  constexpr expected& operator=(const expected&) = default;
  constexpr expected& operator=(expected&&) = default;

  template<typename U = T, std::enable_if_t<conjunction<
      negation<std::is_void<T>>,
      negation<std::is_same<expected, remove_cvref_t<U>>>,
      negation<is_specialization<remove_cvref_t<U>, unexpected>>,
      std::is_constructible<T, U>,
      std::is_assignable<std::add_lvalue_reference_t<T>, U>,
      disjunction<
          std::is_nothrow_constructible<T, U>,
          std::is_nothrow_move_constructible<T>,
          std::is_nothrow_move_constructible<E>
      >
  >::value, int> = 0>
  constexpr expected& operator=(U&& v) {
    if (has_value()) {
      **this = std::forward<U>(v);
    } else {
      base::reinit_value_and_destroy_error(std::forward<U>(v));
      base::has_value(true);
    }
    return *this;
  }

  template<typename G, std::enable_if_t<conjunction<
      std::is_constructible<E, const G&>,
      std::is_assignable<E&, const G&>,
      disjunction<
          std::is_void<T>,
          std::is_nothrow_constructible<E, const G&>,
          std::is_nothrow_move_constructible<T>,
          std::is_nothrow_move_constructible<E>
      >
  >::value, int> = 0>
  constexpr expected& operator=(const unexpected<G>& e) {
    if (has_value()) {
      base::reinit_error_and_destroy_value(e.error());
      base::has_value(false);
    } else {
      error() = e.error();
    }

    return *this;
  }

  template<typename G, std::enable_if_t<conjunction<
      std::is_constructible<E, G>,
      std::is_assignable<E&, G>,
      disjunction<
          std::is_void<T>,
          std::is_nothrow_constructible<E, G>,
          std::is_nothrow_move_constructible<T>,
          std::is_nothrow_move_constructible<E>
      >
  >::value, int> = 0>
  constexpr expected& operator=(unexpected<G>&& e) {
    if (has_value()) {
      base::reinit_error_and_destroy_value(std::move(e.error()));
      base::has_value(false);
    } else {
      error() = std::move(e.error());
    }

    return *this;
  }

  template<typename U = T, std::enable_if_t<!std::is_void<U>::value, int> = 0>
  constexpr const T* operator->() const noexcept {
    return preview::addressof(base::value());
  }

  template<typename U = T, std::enable_if_t<!std::is_void<U>::value, int> = 0>
  constexpr T* operator->() noexcept {
    return preview::addressof(base::value());
  }

  template<typename U = T, std::enable_if_t<!std::is_void<U>::value, int> = 0>
  constexpr const U& operator*() const& noexcept {
    return base::value();
  }

  template<typename U = T, std::enable_if_t<!std::is_void<U>::value, int> = 0>
  constexpr U& operator*() & noexcept {
    return base::value();
  }

  template<typename U = T, std::enable_if_t<!std::is_void<U>::value, int> = 0>
  constexpr const U&& operator*() const&& noexcept {
    return std::move(base::value());
  }

  template<typename U = T, std::enable_if_t<!std::is_void<U>::value, int> = 0>
  constexpr U&& operator*() && noexcept {
    return std::move(base::value());
  }

  template<typename U = T, std::enable_if_t<std::is_void<U>::value, int> = 0>
  constexpr void operator*() & noexcept {}
  template<typename U = T, std::enable_if_t<std::is_void<U>::value, int> = 0>
  constexpr void operator*() const& noexcept {}
  template<typename U = T, std::enable_if_t<std::is_void<U>::value, int> = 0>
  constexpr void operator*() && noexcept {}
  template<typename U = T, std::enable_if_t<std::is_void<U>::value, int> = 0>
  constexpr void operator*() const&& noexcept {}

  PREVIEW_NODISCARD bool has_value() const noexcept {
    return base::has_value();
  }

  constexpr explicit operator bool() const noexcept {
    return has_value();
  }

  template<typename U = T, std::enable_if_t<!std::is_void<U>::value, int> = 0>
  constexpr U& value() & {
    check_throw();
    return **this;
  }

  template<typename U = T, std::enable_if_t<!std::is_void<U>::value, int> = 0>
  constexpr const U& value() const & {
    check_throw();
    return **this;
  }

  template<typename U = T, std::enable_if_t<!std::is_void<U>::value, int> = 0>
  constexpr U&& value() && {
    check_throw();
    return std::move(**this);
  }

  template<typename U = T, std::enable_if_t<!std::is_void<U>::value, int> = 0>
  constexpr const U&& value() const && {
    check_throw();
    return std::move(**this);
  }

  template<typename U = T, std::enable_if_t<std::is_void<U>::value, int> = 0>
  constexpr void value() const & {
    check_throw();
  }

  template<typename U = T, std::enable_if_t<std::is_void<U>::value, int> = 0>
  constexpr void value() && {
    check_throw();
  }

  constexpr const E& error() const& noexcept { return base::error(); }
  constexpr E& error() & noexcept { return base::error(); }
  constexpr const E&& error() const&& noexcept { return std::move(base::error()); }
  constexpr E&& error() && noexcept { return std::move(base::error()); }

  template<typename U, std::enable_if_t<conjunction<
      negation<std::is_void<T>>,
      copy_constructible<T>,
      convertible_to<U&&, T>
  >::value, int> = 0>
  constexpr T value_or(U&& default_value) const & {
    return bool(*this) ? **this : static_cast<T>(std::forward<U>(default_value));
  }

  template<typename U, std::enable_if_t<conjunction<
      negation<std::is_void<T>>,
      move_constructible<T>,
      convertible_to<U&&, T>
  >::value, int> = 0>
  constexpr T value_or(U&& default_value) && {
    return bool(*this) ? std::move(**this) : static_cast<T>(std::forward<U>(default_value));
  }

  template<typename G = E, std::enable_if_t<conjunction<
      std::is_copy_constructible<E>,
      std::is_convertible<G, E>
  >::value, int> = 0>
  constexpr E error_or(G&& default_value) const & {
    return has_value() ? std::forward<G>(default_value) : error();
  }

  template<typename G = E, std::enable_if_t<conjunction<
      std::is_move_constructible<E>,
      std::is_convertible<G, E>
  >::value, int> = 0>
  constexpr E error_or(G&& default_value) && {
    return has_value() ? std::forward<G>(default_value) : std::move(error());
  }

  template<typename F, std::enable_if_t<conjunction<
      std::is_constructible<E, E&>,
      is_invocable<and_then_t, F, expected&>
  >::value, int> = 0>
  constexpr auto and_then(F&& f) & {
    return and_then_t{}(std::forward<F>(f), *this);
  }

  template<typename F, std::enable_if_t<conjunction<
      std::is_constructible<E, const E&>,
      is_invocable<and_then_t, F, const expected&>
  >::value, int> = 0>
  constexpr auto and_then(F&& f) const & {
    return and_then_t{}(std::forward<F>(f), *this);
  }

  template<typename F, std::enable_if_t<conjunction<
      std::is_constructible<E, E&&>,
      is_invocable<and_then_t, F, expected&&>
  >::value, int> = 0>
  constexpr auto and_then(F&& f) && {
    return and_then_t{}(std::forward<F>(f), std::move(*this));
  }

  template<typename F, std::enable_if_t<conjunction<
      std::is_constructible<E, const E&&>,
      is_invocable<and_then_t, F, const expected&&>
  >::value, int> = 0>
  constexpr auto and_then(F&& f) const && {
    return and_then_t{}(std::forward<F>(f), std::move(*this));
  }

  template<typename F, std::enable_if_t<conjunction<
      std::is_constructible<E, E&>,
      is_invocable<transform_t, F, expected&>
  >::value, int> = 0>
  constexpr auto transform(F&& f) & {
    return transform_t{}(std::forward<F>(f), *this);
  }

  template<typename F, std::enable_if_t<conjunction<
      std::is_constructible<E, const E&>,
      is_invocable<transform_t, F, const expected&>
  >::value, int> = 0>
  constexpr auto transform(F&& f) const & {
    return transform_t{}(std::forward<F>(f), *this);
  }

  template<typename F, std::enable_if_t<conjunction<
      std::is_constructible<E, E&&>,
      is_invocable<transform_t, F, expected&&>
  >::value, int> = 0>
  constexpr auto transform(F&& f) && {
    return transform_t{}(std::forward<F>(f), std::move(*this));
  }

  template<typename F, std::enable_if_t<conjunction<
      std::is_constructible<E, const E&&>,
      is_invocable<transform_t, F, const expected&&>
  >::value, int> = 0>
  constexpr auto transform(F&& f) const && {
    return transform_t{}(std::forward<F>(f), std::move(*this));
  }

  template<typename F, std::enable_if_t<conjunction<
      detail::void_or<T, std::is_constructible, T, std::add_lvalue_reference_t<T>>,
      is_invocable<or_else_t, F, expected&>
  >::value, int> = 0>
  constexpr auto or_else(F&& f) & {
    return or_else_t{}(std::forward<F>(f), *this);
  }

  template<typename F, std::enable_if_t<conjunction<
      detail::void_or<T, std::is_constructible, T, std::add_lvalue_reference_t<std::add_const_t<T>>>,
      is_invocable<or_else_t, F, const expected&>
  >::value, int> = 0>
  constexpr auto or_else(F&& f) const & {
    return or_else_t{}(std::forward<F>(f), *this);
  }

  template<typename F, std::enable_if_t<conjunction<
      detail::void_or<T, std::is_constructible, T, std::add_rvalue_reference_t<T>>,
      is_invocable<or_else_t, F, expected&&>
  >::value, int> = 0>
  constexpr auto or_else(F&& f) && {
    return or_else_t{}(std::forward<F>(f), std::move(*this));
  }

  template<typename F, std::enable_if_t<conjunction<
      detail::void_or<T, std::is_constructible, T, std::add_rvalue_reference_t<std::add_const_t<T>>>,
      is_invocable<or_else_t, F, const expected&&>
  >::value, int> = 0>
  constexpr auto or_else(F&& f) const && {
    return or_else_t{}(std::forward<F>(f), std::move(*this));
  }

  template<typename F, std::enable_if_t<conjunction<
      detail::void_or<T, std::is_constructible, T, std::add_lvalue_reference_t<T>>,
      is_invocable<transform_error_t, F, expected&>
  >::value, int> = 0>
  constexpr auto transform_error(F&& f) & {
    return transform_error_t{}(std::forward<F>(f), *this);
  }

  template<typename F, std::enable_if_t<conjunction<
      detail::void_or<T, std::is_constructible, T, std::add_lvalue_reference_t<std::add_const_t<T>>>,
      is_invocable<transform_error_t, F, const expected&>
  >::value, int> = 0>
  constexpr auto transform_error(F&& f) const & {
    return transform_error_t{}(std::forward<F>(f), *this);
  }

  template<typename F, std::enable_if_t<conjunction<
      detail::void_or<T, std::is_constructible, T, std::add_rvalue_reference_t<T>>,
      is_invocable<transform_error_t, F, expected&&>
  >::value, int> = 0>
  constexpr auto transform_error(F&& f) && {
    return transform_error_t{}(std::forward<F>(f), std::move(*this));
  }

  template<typename F, std::enable_if_t<conjunction<
      detail::void_or<T, std::is_constructible, T, std::add_rvalue_reference_t<std::add_const_t<T>>>,
      is_invocable<transform_error_t, F, const expected&&>
  >::value, int> = 0>
  constexpr auto transform_error(F&& f) const && {
    return transform_error_t{}(std::forward<F>(f), std::move(*this));
  }

  template<typename... Args, std::enable_if_t<conjunction<
      negation<std::is_void<T>>,
      std::is_nothrow_constructible<T, Args...>
  >::value, int> = 0>
  constexpr std::add_lvalue_reference_t<T> emplace(Args&&... args) noexcept {
    if (has_value()) {
      detail::reinit_expected(value(), value(), std::forward<Args>(args)...);
    } else {
      base::reinit_value_and_destroy_error(std::forward<Args>(args)...);
    }
    return value();
  }

  template<typename U, typename... Args, std::enable_if_t<conjunction<
      negation<std::is_void<T>>,
      std::is_nothrow_constructible<T, std::initializer_list<U>&, Args...>
  >::value, int> = 0>
  constexpr std::add_lvalue_reference_t<T> emplace(std::initializer_list<U> il, Args&&... args) noexcept {
    if (has_value()) {
      detail::reinit_expected(value(), value(), il, std::forward<Args>(args)...);
    } else {
      base::reinit_value_and_destroy_error(il, std::forward<Args>(args)...);
    }
    return value();
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<
      std::is_void<Dummy>,
      std::is_void<T>
  >::value, int> = 0>
  constexpr void emplace() noexcept {
    if(has_value())
      base::destroy_value();
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<
      std::is_void<Dummy>,
      detail::void_or<T, is_swappable, T>,
      is_swappable<E>,
      detail::void_or<T, std::is_move_constructible, T>,
      std::is_move_constructible<E>,
      disjunction<
          detail::void_or<T, std::is_nothrow_move_constructible, T>,
          std::is_nothrow_move_constructible<E>
      >
  >::value, int> = 0>
  constexpr void swap(expected& other) noexcept(nothrow_swappable::value) {
    if (has_value() == other.has_value()) {
      using std::swap;
      using preview::swap;
      has_value() ? swap_value(**this, *other) : swap(error(), other.error());
    } else {
      has_value() ? swap_value_with_error(*this, other) : swap_value_with_error(other, *this);
    }
  }

  template<typename T2, typename E2, std::enable_if_t<conjunction<
      negation<std::is_void<T2>>,
      negation<std::is_void<T>>,
      rel_ops::is_equality_comparable<
          std::add_lvalue_reference_t<std::add_const_t<T>>,
          std::add_lvalue_reference_t<std::add_const_t<T2>>>,
      rel_ops::is_equality_comparable<const E&, const E2&>
  >::value, int> = 0>
  friend constexpr bool operator==(const expected& lhs, const expected<T2, E2>& rhs) {
    if (lhs.has_value() != rhs.has_value())
      return false;
    using namespace rel_ops;
    return lhs.has_value() ? *lhs == *rhs : lhs.error() == rhs.error();
  }

  template<typename T2, typename E2, std::enable_if_t<conjunction<
      std::is_void<T2>,
      std::is_void<T>,
      rel_ops::is_equality_comparable<const E&, const E2&>
  >::value, int> = 0>
  friend constexpr bool operator==(const expected& lhs, const expected<T2, E2>& rhs) {
    if (lhs.has_value() != rhs.has_value())
      return false;
    using namespace rel_ops;
    return lhs.has_value() ? true : lhs.error() == rhs.error();
  }

  template<typename T2, typename E2, std::enable_if_t<rel_ops::is_equality_comparable<const expected&, const expected<T2, E2>&>::value, int> = 0>
  friend constexpr bool operator!=(const expected& lhs, const expected<T2, E2>& rhs) {
    return !(lhs == rhs);
  }

  template<typename T2, std::enable_if_t<conjunction<
      negation<std::is_void<T>>,
      rel_ops::is_equality_comparable<std::add_lvalue_reference_t<std::add_const_t<T>>, const T2&>
  >::value, int> = 0>
  friend constexpr bool operator==(const expected& x, const T2& y) {
    using namespace rel_ops;
    return x.has_value() && static_cast<bool>(*x == y);
  }

  template<typename T2, std::enable_if_t<rel_ops::is_equality_comparable<const expected&, const T2&>::value, int> = 0>
  friend constexpr bool operator!=(const expected& x, const T2& y) {
    return !(x == y);
  }

  template<typename T2, std::enable_if_t<rel_ops::is_equality_comparable<const expected&, const T2&>::value, int> = 0>
  friend constexpr bool operator==(const T2& y, const expected& x) {
    return x == y;
  }

  template<typename T2, std::enable_if_t<rel_ops::is_equality_comparable<const expected&, const T2&>::value, int> = 0>
  friend constexpr bool operator!=(const T2& y, const expected& x) {
    return !(x == y);
  }

  template<typename E2, std::enable_if_t<rel_ops::is_equality_comparable<const E&, const E2&>::value, int> = 0>
  friend constexpr bool operator==(const expected& x, const unexpected<E2>& e) {
    using namespace rel_ops;
    return !x.has_value() && static_cast<bool>(x.error() == e.error());
  }

  template<typename E2, std::enable_if_t<rel_ops::is_equality_comparable<const E&, const E2&>::value, int> = 0>
  friend constexpr bool operator!=(const expected& x, const unexpected<E2>& e) {
    return !(x == e);
  }

  template<typename E2, std::enable_if_t<rel_ops::is_equality_comparable<const E&, const E2&>::value, int> = 0>
  friend constexpr bool operator==(const unexpected<E2>& e, const expected& x) {
    return x == e;
  }

  template<typename E2, std::enable_if_t<rel_ops::is_equality_comparable<const E&, const E2&>::value, int> = 0>
  friend constexpr bool operator!=(const unexpected<E2>& e, const expected& x) {
    return !(x == e);
  }

 private:
  static constexpr
  void swap_value(expected&, expected&, std::true_type /* void */) noexcept {}

  static constexpr
  void swap_value(expected& thiz, expected& other, std::false_type /* void */) noexcept(value_nothrow_swappable::value) {
    using std::swap;
    using preview::swap;
    swap(*thiz, *other);
  }

  static PREVIEW_CONSTEXPR_AFTER_CXX17
  void swap_value_with_error(expected& thiz, expected& other) {
    expected::swap_value_with_error(thiz, other, std::is_void<T>{}, std::is_nothrow_move_constructible<E>{});
  }

  template<typename Any>
  static PREVIEW_CONSTEXPR_AFTER_CXX17
  void swap_value_with_error(expected& thiz, expected& other, std::true_type /* void */, Any) noexcept(error_nothrow_swappable::value) {
    preview::construct_at(std::addressof(thiz.error()), std::move(other.error()));
    preview::destroy_at(std::addressof(other.error()));
  }

  static PREVIEW_CONSTEXPR_AFTER_CXX23
  void swap_value_with_error(expected& thiz, expected& other, std::false_type /* void */, std::true_type /* nothrow */) noexcept(nothrow_swappable::value) {
    E temp(std::move(other.error()));
    preview::destroy_at(std::addressof(other.error()));
    try {
      preview::construct_at(std::addressof(other.value()), std::move(thiz.value()));
      preview::destroy_at(std::addressof(thiz.value()));
      preview::construct_at(std::addressof(thiz.error()), std::move(temp));
    } catch (...) {
      preview::construct_at(std::addressof(other.error()), std::move(temp));
      throw;
    }
  }

  static PREVIEW_CONSTEXPR_AFTER_CXX23
  void swap_value_with_error(expected& thiz, expected& other, std::false_type /* void */, std::false_type /* nothrow */) noexcept(nothrow_swappable::value) {
    T temp(std::move(thiz.value()));
    preview::destroy_at(std::addressof(thiz.value()));
    try {
      preview::construct_at(std::addressof(thiz.error()), std::move(other.error()));
      preview::destroy_at(std::addressof(other.error()));
      preview::construct_at(std::addressof(other.value()), std::move(temp));
    } catch (...) {
      preview::construct_at(std::addressof(thiz.value()), std::move(temp));
      throw;
    }
  }

  constexpr void check_throw() & {
    if (!has_value())
      throw bad_expected_access<std::decay_t<E>>(preview::as_const(error()));
  }
  constexpr void check_throw() && {
    if (!has_value())
      throw bad_expected_access<std::decay_t<E>>(std::move(error()));
  }
};

} // namespace preview

#endif // PREVIEW_EXPECTED_EXPECTED_H_
