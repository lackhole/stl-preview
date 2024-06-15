//
// Created by yonggyulee on 2024/01/02.
//

#ifndef PREVIEW_RANGES_MOVABLE_BOX_H_
#define PREVIEW_RANGES_MOVABLE_BOX_H_

#include <initializer_list>
#include <type_traits>
#include <utility>

#include "preview/__concepts/copyable.h"
#include "preview/__concepts/copy_constructible.h"
#include "preview/__concepts/movable.h"
#include "preview/__memory/addressof.h"
#include "preview/optional.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace detail {

template<typename T>
struct movable_box_store_obj_only
    : disjunction<
          copyable<T>,
          conjunction<
              copy_constructible<T>,
              std::is_nothrow_move_constructible<T>,
              std::is_nothrow_copy_constructible<T> >,
          conjunction<
              negation< copy_constructible<T> >,
              movable<T> >,
          conjunction<
              negation< copy_constructible<T> >,
              std::is_nothrow_copy_constructible<T> >
      > {};

template<typename T, bool = movable_box_store_obj_only<T>::value>
struct movable_box_storage_base;

template<typename T>
struct movable_box_storage_base<T, true> {

  constexpr movable_box_storage_base() = default;
  constexpr movable_box_storage_base(const movable_box_storage_base&) = default;
  constexpr movable_box_storage_base(movable_box_storage_base&&) = default;
  constexpr movable_box_storage_base& operator=(const movable_box_storage_base&) = default;
  constexpr movable_box_storage_base& operator=(movable_box_storage_base&&) = default;

  template<typename InPlace, std::enable_if_t<conjunction<
      std::is_same<in_place_t, InPlace>,
      std::is_constructible<T>
  >::value, int> = 0>
  constexpr movable_box_storage_base(InPlace)
      noexcept(std::is_nothrow_default_constructible<T>::value)
      : value_() {}

  template<typename U, std::enable_if_t<conjunction<
      negation< std::is_same<in_place_t, remove_cvref_t<U>> >,
      negation< std::is_same<movable_box_storage_base, remove_cvref_t<U>> >,
      std::is_constructible<T, U>
  >::value, int> = 0>
  constexpr movable_box_storage_base(U&& arg)
      noexcept(std::is_nothrow_constructible<T, U>::value)
      : value_(std::forward<U>(arg)) {}

  template<typename Arg, typename... Args, std::enable_if_t<conjunction<
      std::is_constructible<T, Arg, Args...>
  >::value, int> = 0>
  constexpr movable_box_storage_base(in_place_t, Arg&& arg, Args&&... args)
      noexcept(std::is_nothrow_constructible<T, Arg, Args...>::value)
      : value_(std::forward<Arg>(arg), std::forward<Args>(args)...) {}

  template<typename U, typename... Args, std::enable_if_t<conjunction<
      std::is_constructible<T, std::initializer_list<U>, Args...>
  >::value, int> = 0>
  constexpr movable_box_storage_base(in_place_t, std::initializer_list<U> il, Args&&... args)
      noexcept(std::is_nothrow_constructible<T, std::initializer_list<U>, Args...>::value)
      : value_(il, std::forward<Args>(args)...) {}

  template<typename U, std::enable_if_t<conjunction<
      negation< std::is_same<movable_box_storage_base, std::decay_t<U>> >,
      std::is_assignable<T, U>
  >::value, int> = 0>
  constexpr movable_box_storage_base& operator=(U&& arg) noexcept(std::is_nothrow_assignable<T, U>::value) {
    value_ = std::forward<U>(arg);
    return *this;
  }

  constexpr explicit operator bool() const noexcept { return true; }
  constexpr bool has_value() const noexcept { return true; }

  constexpr const T* operator->() const noexcept { return preview::addressof(value_); }
  constexpr T* operator->() noexcept { return preview::addressof(value_); }
  constexpr const T& operator*() const& noexcept { return value_; }
  constexpr T& operator*() & noexcept { return value_; }
  constexpr const T&& operator*() const&& noexcept { return std::move(value_); }
  constexpr T&& operator*() && noexcept { return std::move(value_); }

  constexpr void reset() noexcept { value_.~T(); }

  template<typename... Args, std::enable_if_t<std::is_constructible<T, Args...>::value, int> = 0>
  constexpr T& emplace(Args&&... args)
      noexcept(conjunction< std::is_nothrow_constructible<T, Args...>,
                            std::is_nothrow_destructible<T> >::value)
  {
    value_.~T();
    ::new(this->operator->()) T(std::forward<Args>(args)...);
    return value_;
  }

  template<typename U, typename... Args, std::enable_if_t<std::is_constructible<T, std::initializer_list<U>, Args...>::value, int> = 0>
  constexpr T& emplace(std::initializer_list<U> il, Args&&... args)
      noexcept(conjunction< std::is_nothrow_constructible<T, std::initializer_list<U>, Args...>,
                            std::is_nothrow_destructible<T> >::value)
  {
    value_.~T();
    ::new(this->operator->()) T(il, std::forward<Args>(args)...);
    return value_;
  }

 private:
  T value_;
};

template<typename T>
struct movable_box_storage_base<T, false> : private optional<T> {
  using base = optional<T>;

  using base::base;
  using base::operator=;
  using base::operator->;
  using base::operator*;
  using base::operator bool;
  using base::has_value;
  using base::reset;
  using base::emplace;
};

template<typename T, bool = conjunction<copyable<T>, copy_constructible<T>>::value /* true */>
struct movable_box_storage_copy_assign : movable_box_storage_base<T> {
  using base = movable_box_storage_base<T>;
  using base::base;
};

template<typename T>
struct movable_box_storage_copy_assign<T, false> : movable_box_storage_base<T> {
  using base = movable_box_storage_base<T>;
  using base::base;

  constexpr movable_box_storage_copy_assign() = default;
  constexpr movable_box_storage_copy_assign(const movable_box_storage_copy_assign&) = default;
  constexpr movable_box_storage_copy_assign(movable_box_storage_copy_assign&&) = default;

  movable_box_storage_copy_assign& operator=(const movable_box_storage_copy_assign& other)
      noexcept(std::is_nothrow_copy_constructible<T>::value)
  {
    if (this != preview::addressof(other)) {
      if (other)
        this->emplace(*other);
      else
        this->reset();
    }
    return *this;
  }
  constexpr movable_box_storage_copy_assign& operator=(movable_box_storage_copy_assign&&) = default;
};

template<typename T, bool = movable<T>::value /* true */>
struct movable_box_storage_move_assign : movable_box_storage_copy_assign<T> {
  using base = movable_box_storage_copy_assign<T>;
  using base::base;

  constexpr movable_box_storage_move_assign() = default;
  constexpr movable_box_storage_move_assign(const movable_box_storage_move_assign&) = default;
  constexpr movable_box_storage_move_assign(movable_box_storage_move_assign&&) = default;
  constexpr movable_box_storage_move_assign& operator=(const movable_box_storage_move_assign&) = default;
  constexpr movable_box_storage_move_assign& operator=(movable_box_storage_move_assign&& other) = default;
};

template<typename T>
struct movable_box_storage_move_assign<T, false> : movable_box_storage_copy_assign<T> {
  using base = movable_box_storage_copy_assign<T>;
  using base::base;

  constexpr movable_box_storage_move_assign() = default;
  constexpr movable_box_storage_move_assign(const movable_box_storage_move_assign&) = default;
  constexpr movable_box_storage_move_assign(movable_box_storage_move_assign&&) = default;
  constexpr movable_box_storage_move_assign& operator=(const movable_box_storage_move_assign&) = default;
  constexpr movable_box_storage_move_assign& operator=(movable_box_storage_move_assign&& other)
      noexcept(std::is_nothrow_move_constructible<T>::value)
  {
    if (this != preview::addressof(other)) {
      if (other)
        this->emplace(std::move(*other));
      else
        this->reset();
    }
    return *this;
  }
};

} // namespace detail

template<typename T>
class movable_box : public detail::movable_box_storage_move_assign<T> {
  using base = detail::movable_box_storage_move_assign<T>;
  using base::base;

 public:
  static_assert(move_constructible<T>::value, "Constraints not satisfied");
  static_assert(std::is_object<T>::value, "Constraints not satisfied");

  constexpr movable_box() = default;
  constexpr movable_box(const movable_box&) = default;
  constexpr movable_box(movable_box&&) = default;
  constexpr movable_box& operator=(const movable_box&) = default;
  constexpr movable_box& operator=(movable_box&&) = default;
};

} // namespace preview

#endif // PREVIEW_RANGES_MOVABLE_BOX_H_
