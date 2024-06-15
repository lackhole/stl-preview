//
// Created by yonggyulee on 2/1/24.
//

#ifndef PREVIEW_TYPE_TRAITS_CORE_CONTROL_SPECIAL_H_
#define PREVIEW_TYPE_TRAITS_CORE_CONTROL_SPECIAL_H_

#include <type_traits>

#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace detail {

template<typename Base>
struct non_trivial_copy_ctor : Base {
  using base = Base;
  using base::base;

  non_trivial_copy_ctor() = default;
  non_trivial_copy_ctor(const non_trivial_copy_ctor& other)
      noexcept(noexcept(std::declval<Base&>().construct_from(static_cast<const Base&>(other))))
  {
    Base::construct_from(static_cast<const Base&>(other));
  }
  non_trivial_copy_ctor(non_trivial_copy_ctor&&) = default;
  non_trivial_copy_ctor& operator=(const non_trivial_copy_ctor&) = default;
  non_trivial_copy_ctor& operator=(non_trivial_copy_ctor&&) = default;
};

template<typename Base>
struct deleted_copy_ctor : Base {
  using base = Base;
  using base::base;

  deleted_copy_ctor() = default;
  deleted_copy_ctor(const deleted_copy_ctor&) = delete;
  deleted_copy_ctor(deleted_copy_ctor&&) = default;
  deleted_copy_ctor& operator=(const deleted_copy_ctor&) = default;
  deleted_copy_ctor& operator=(deleted_copy_ctor&&) = default;
};

template<typename Base, typename... T>
using control_copy_ctor =
    std::conditional_t<
        conjunction<std::is_trivially_copy_constructible<T>...>::value, Base,
    std::conditional_t<
        conjunction<std::is_copy_constructible<T>...>::value, non_trivial_copy_ctor<Base>,
        deleted_copy_ctor<Base>
    >>;

template<typename Base, typename... T>
struct non_trivial_move_ctor : control_copy_ctor<Base, T...> {
  using base = control_copy_ctor<Base, T...>;
  using base::base;

  non_trivial_move_ctor() = default;
  non_trivial_move_ctor(const non_trivial_move_ctor&) = default;
  non_trivial_move_ctor(non_trivial_move_ctor&& other)
      noexcept(noexcept(std::declval<Base&>().construct_from(static_cast<Base&&>(other))))
  {
    Base::construct_from(static_cast<Base&&>(other));
  }
  non_trivial_move_ctor& operator=(const non_trivial_move_ctor&) = default;
  non_trivial_move_ctor& operator=(non_trivial_move_ctor&&) = default;
};

template<typename Base, typename... T>
struct deleted_move_ctor : control_copy_ctor<Base, T...> {
  using base = control_copy_ctor<Base, T...>;
  using base::base;

  deleted_move_ctor() = default;
  deleted_move_ctor(const deleted_move_ctor&) = default;
  deleted_move_ctor(deleted_move_ctor&&) = delete;
  deleted_move_ctor& operator=(const deleted_move_ctor&) = default;
  deleted_move_ctor& operator=(deleted_move_ctor&&) = default;
};

template<typename Base, typename... T>
using control_move_ctor =
    std::conditional_t<
        conjunction<std::is_trivially_move_constructible<T>...>::value, control_copy_ctor<Base, T...>,
    std::conditional_t<
        conjunction<std::is_move_constructible<T>...>::value, non_trivial_move_ctor<Base, T...>,
        deleted_move_ctor<Base, T...>
    >>;

template<typename Base, typename... T>
struct non_trivial_copy_assign : control_move_ctor<Base, T...> {
  using base = control_move_ctor<Base, T...>;
  using base::base;

  non_trivial_copy_assign() = default;
  non_trivial_copy_assign(const non_trivial_copy_assign& other) = default;
  non_trivial_copy_assign(non_trivial_copy_assign&&) = default;
  non_trivial_copy_assign& operator=(const non_trivial_copy_assign& other)
      noexcept(noexcept(std::declval<Base&>().assign_from(static_cast<const Base&>(other))))
  {
    Base::assign_from(static_cast<const Base&>(other));
    return *this;
  }
  non_trivial_copy_assign& operator=(non_trivial_copy_assign&&) = default;
};

template<typename Base, typename... T>
struct deleted_copy_assign : control_move_ctor<Base, T...> {
  using base = control_move_ctor<Base, T...>;
  using base::base;

  deleted_copy_assign() = default;
  deleted_copy_assign(const deleted_copy_assign&) = default;
  deleted_copy_assign(deleted_copy_assign&&) = default;
  deleted_copy_assign& operator=(const deleted_copy_assign&) = delete;
  deleted_copy_assign& operator=(deleted_copy_assign&&) = default;
};

template<typename Base, typename... T>
using control_copy_assign =
    std::conditional_t<
        conjunction<
            std::is_trivially_copy_constructible<T>...,
            std::is_trivially_copy_assignable<T>...,
            std::is_trivially_destructible<T>...
        >::value,
        control_move_ctor<Base, T...>,
    std::conditional_t<
        conjunction<
            std::is_copy_constructible<T>...,
            std::is_copy_assignable<T>...
        >::value,
        non_trivial_copy_assign<Base, T...>,
        deleted_copy_assign<Base, T...>
    >>;

template<typename Base, typename... T>
struct non_trivial_move_assign : control_copy_assign<Base, T...> {
  using base = control_copy_assign<Base, T...>;
  using base::base;

  non_trivial_move_assign() = default;
  non_trivial_move_assign(const non_trivial_move_assign& other) = default;
  non_trivial_move_assign(non_trivial_move_assign&&) = default;
  non_trivial_move_assign& operator=(const non_trivial_move_assign&) = default;
  non_trivial_move_assign& operator=(non_trivial_move_assign&& other)
      noexcept(noexcept(std::declval<Base&>().assign_from(static_cast<Base&&>(other))))
  {
    Base::assign_from(static_cast<Base&&>(other));
    return *this;
  }
};

template<typename Base, typename... T>
struct deleted_move_assign : control_copy_assign<Base, T...> {
  using base = control_copy_assign<Base, T...>;
  using base::base;

  deleted_move_assign() = default;
  deleted_move_assign(const deleted_move_assign&) = default;
  deleted_move_assign(deleted_move_assign&&) = default;
  deleted_move_assign& operator=(const deleted_move_assign&) = default;
  deleted_move_assign& operator=(deleted_move_assign&&) = delete;
};

template<typename Base, typename... T>
using control_move_assign =
    std::conditional_t<
        conjunction<
            std::is_trivially_move_constructible<T>...,
            std::is_trivially_move_assignable<T>...,
            std::is_trivially_destructible<T>...
        >::value,
        control_copy_assign<Base, T...>,
    std::conditional_t<
        conjunction<
            std::is_move_constructible<T>...,
            std::is_move_assignable<T>...
        >::value,
        non_trivial_move_assign<Base, T...>,
        deleted_move_assign<Base, T...>
    >>;

// DtorBase must define construct_from and assign_from
template<typename DtorBase, typename... T>
using control_special = control_move_assign<DtorBase, T...>;

} // namespace detail
} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_CORE_CONTROL_SPECIAL_H_
