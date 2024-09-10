//
// Created by YongGyu Lee on 10/27/23.
//

#ifndef PREVIEW_VARIANT_VARIANT_H_
#define PREVIEW_VARIANT_VARIANT_H_

#include <cassert>
#include <new>
#include <type_traits>

#include "preview/__concepts/different_from.h"
#include "preview/__concepts/equality_comparable.h"
#include "preview/__concepts/totally_ordered.h"
#include "preview/__core/constexpr.h"
#include "preview/__functional/equal_to.h"
#include "preview/__functional/not_equal_to.h"
#include "preview/__functional/greater.h"
#include "preview/__functional/greater_equal.h"
#include "preview/__functional/invoke.h"
#include "preview/__functional/less.h"
#include "preview/__functional/less_equal.h"
#include "preview/__memory/addressof.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/detail/control_special.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/is_list_initializable.h"
#include "preview/__type_traits/is_specialization.h"
#include "preview/__type_traits/is_swappable.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/type_identity.h"
#include "preview/__utility/type_sequence.h"
#include "preview/__utility/in_place.h"
#include "preview/__variant/bad_variant_access.h"
#include "preview/__variant/monostate.h"
#include "preview/__variant/variant_alternative.h"
#include "preview/__variant/variant_npos.h"

namespace preview {

template<typename... Types>
class variant;

namespace detail {

template<typename T>
struct index_value;
template<std::size_t I>
struct index_value<in_place_index_t<I>> : std::integral_constant<std::size_t, I> {};

template<bool IsTriviallyDestructible, typename... Types>
struct union_wrapper_t {};

template<typename... Types>
using union_wrapper = union_wrapper_t<conjunction<std::is_trivially_destructible<Types>...>::value, Types...>;

template<typename T, typename... Types>
struct union_wrapper_t<true, T, Types...> {
  constexpr union_wrapper_t() noexcept {}

  template<typename ...U>
  constexpr explicit union_wrapper_t(in_place_index_t<0>, U&&... args)
      noexcept(std::is_nothrow_constructible<T, U...>::value)
      : head_(std::forward<U>(args)...) {}

  template<std::size_t I, typename ...U>
  constexpr explicit union_wrapper_t(in_place_index_t<I>, U&&... args)
      noexcept(std::is_nothrow_constructible<union_wrapper<Types...>, in_place_index_t<I - 1>, U...>::value)
      : tail_(in_place_index<I - 1>, std::forward<U>(args)...) {}

  static constexpr std::size_t size = sizeof...(Types) + 1;

  constexpr       T&  get()       & noexcept { return head_; }
  constexpr const T&  get()  const& noexcept { return head_; }
  constexpr       T&& get()      && noexcept { return std::move(head_); }
  constexpr const T&& get() const&& noexcept { return std::move(head_); }

  union {
    T head_;
    union_wrapper<Types...> tail_;
  };
};

template<typename T, typename... Types>
struct union_wrapper_t<false, T, Types...> {
  constexpr union_wrapper_t() noexcept {}

  template<typename ...U>
  constexpr explicit union_wrapper_t(in_place_index_t<0>, U&&... args)
      noexcept(std::is_nothrow_constructible<T, U...>::value)
      : head_(std::forward<U>(args)...) {}

  template<std::size_t I, typename ...U>
  constexpr explicit union_wrapper_t(in_place_index_t<I>, U&&... args)
      noexcept(std::is_nothrow_constructible<union_wrapper<Types...>, in_place_index_t<I - 1>, U...>::value)
      : tail_(in_place_index_t<I - 1>{}, std::forward<U>(args)...) {}

  static constexpr std::size_t size = sizeof...(Types) + 1;

  PREVIEW_CONSTEXPR_AFTER_CXX20 ~union_wrapper_t() noexcept {}

  constexpr union_wrapper_t(const union_wrapper_t&) = default;
  constexpr union_wrapper_t(union_wrapper_t&&) = default;
  constexpr union_wrapper_t& operator=(const union_wrapper_t&) = default;
  constexpr union_wrapper_t& operator=(union_wrapper_t&&) = default;

  constexpr       T&  get()       & noexcept { return head_; }
  constexpr const T&  get()  const& noexcept { return head_; }
  constexpr       T&& get()      && noexcept { return std::move(head_); }
  constexpr const T&& get() const&& noexcept { return std::move(head_); }

  union {
    T head_;
    union_wrapper<Types...> tail_;
  };
};

template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<0>) noexcept { return std::forward<Union>(u).get(); }
template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<1>) noexcept { return std::forward<Union>(u).tail_.get(); }
template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<2>) noexcept { return std::forward<Union>(u).tail_.tail_.get(); }
template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<3>) noexcept { return std::forward<Union>(u).tail_.tail_.tail_.get(); }
template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<4>) noexcept { return std::forward<Union>(u).tail_.tail_.tail_.tail_.get(); }
template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<5>) noexcept { return std::forward<Union>(u).tail_.tail_.tail_.tail_.tail_.get(); }
template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<6>) noexcept { return std::forward<Union>(u).tail_.tail_.tail_.tail_.tail_.tail_.get(); }
template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<7>) noexcept { return std::forward<Union>(u).tail_.tail_.tail_.tail_.tail_.tail_.tail_.get(); }
template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<8>) noexcept { return std::forward<Union>(u).tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.get(); }
template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<9>) noexcept { return std::forward<Union>(u).tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.get(); }
template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<10>) noexcept { return std::forward<Union>(u).tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.get(); }
template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<11>) noexcept { return std::forward<Union>(u).tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.get(); }
template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<12>) noexcept { return std::forward<Union>(u).tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.get(); }
template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<13>) noexcept { return std::forward<Union>(u).tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.get(); }
template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<14>) noexcept { return std::forward<Union>(u).tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.get(); }
template<typename Union> constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<15>) noexcept { return std::forward<Union>(u).tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.get(); }

template<typename Union, std::size_t I>
constexpr decltype(auto) variant_raw_get(Union&& u, in_place_index_t<I>) {
  return variant_raw_get(
      std::forward<decltype(
          std::forward<Union>(u).tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_)>(
          std::forward<Union>(u).tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_
      ),
      in_place_index<I - 16>
  );
}

// visiting npos
template<typename Union, typename F, typename... Args>
constexpr decltype(auto) variant_raw_visit_impl2(Union&& u, F&& func, in_place_index_t<0>, Args&&... args) {
  return preview::invoke(std::forward<F>(func), std::forward<Union>(u), in_place_index<variant_npos>, std::forward<Args>(args)...);
}

template<std::size_t I, typename Union, typename F, typename... Args>
constexpr decltype(auto) variant_raw_visit_impl2(Union&& u, F&& func, in_place_index_t<I>, Args&&... args) {
  return preview::invoke(
      std::forward<F>(func),
      variant_raw_get(std::forward<Union>(u), in_place_index<I - 1>),
      in_place_index<I - 1>,
      std::forward<Args>(args)...
  );
}

template<std::size_t Base, typename Union, typename F, typename... Args>
constexpr decltype(auto) variant_raw_visit_recurse(std::size_t i, Union&& u, F&& func, in_place_index_t<Base>, std::true_type, Args&&... args);
template<std::size_t Base, typename Union, typename F, typename... Args>
constexpr decltype(auto) variant_raw_visit_recurse(std::size_t, Union&& u, F&& func, in_place_index_t<Base>, std::false_type, Args&&... args) {
  // unreachable
  return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<0>, std::forward<Args>(args)...);
}

template<std::size_t Base, typename Union, typename F, typename... Args>
constexpr decltype(auto) variant_raw_visit_impl(std::size_t i, Union&& u, F&& func, Args&&... args) {
  switch (i) {
    case 0: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 0 <= remove_cvref_t<Union>::size ? Base + 0 : 0)>, std::forward<Args>(args)...);
    case 1: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 1 <= remove_cvref_t<Union>::size ? Base + 1 : 0)>, std::forward<Args>(args)...);
    case 2: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 2 <= remove_cvref_t<Union>::size ? Base + 2 : 0)>, std::forward<Args>(args)...);
    case 3: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 3 <= remove_cvref_t<Union>::size ? Base + 3 : 0)>, std::forward<Args>(args)...);
    case 4: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 4 <= remove_cvref_t<Union>::size ? Base + 4 : 0)>, std::forward<Args>(args)...);
    case 5: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 5 <= remove_cvref_t<Union>::size ? Base + 5 : 0)>, std::forward<Args>(args)...);
    case 6: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 6 <= remove_cvref_t<Union>::size ? Base + 6 : 0)>, std::forward<Args>(args)...);
    case 7: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 7 <= remove_cvref_t<Union>::size ? Base + 7 : 0)>, std::forward<Args>(args)...);
    case 8: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 8 <= remove_cvref_t<Union>::size ? Base + 8 : 0)>, std::forward<Args>(args)...);
    case 9: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 9 <= remove_cvref_t<Union>::size ? Base + 9 : 0)>, std::forward<Args>(args)...);
    case 10: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 10 <= remove_cvref_t<Union>::size ? Base + 10 : 0)>, std::forward<Args>(args)...);
    case 11: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 11 <= remove_cvref_t<Union>::size ? Base + 11 : 0)>, std::forward<Args>(args)...);
    case 12: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 12 <= remove_cvref_t<Union>::size ? Base + 12 : 0)>, std::forward<Args>(args)...);
    case 13: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 13 <= remove_cvref_t<Union>::size ? Base + 13 : 0)>, std::forward<Args>(args)...);
    case 14: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 14 <= remove_cvref_t<Union>::size ? Base + 14 : 0)>, std::forward<Args>(args)...);
    case 15: return variant_raw_visit_impl2(std::forward<Union>(u), std::forward<F>(func), in_place_index<(Base + 15 <= remove_cvref_t<Union>::size ? Base + 15 : 0)>, std::forward<Args>(args)...);
    default:
      return variant_raw_visit_recurse(
          i - 16, std::forward<Union>(u), std::forward<F>(func),
          in_place_index<Base + 16>,
          bool_constant<(Base + 16 <= remove_cvref_t<Union>::size)>{},
          std::forward<Args>(args)...);
  }
}

template<std::size_t Base, typename Union, typename F, typename... Args>
constexpr decltype(auto) variant_raw_visit_recurse(std::size_t i, Union&& u, F&& func, in_place_index_t<Base>, std::true_type, Args&&... args) {
  return variant_raw_visit_impl<Base>(
      i - 16,
      std::forward<decltype(
          std::forward<Union>(u).tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_)>(
          std::forward<Union>(u).tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_.tail_),
      std::forward<F>(func),
      std::forward<Args>(args)...
  );
}

template<typename Union, typename F, typename... Args>
constexpr decltype(auto) variant_raw_visit(std::size_t i, Union&& u, F&& func, Args&&... args) {
  // Move npos to 0 to avoid infinite switch recursion
  return variant_raw_visit_impl<0>(i + 1, std::forward<Union>(u), std::forward<F>(func), std::forward<Args>(args)...);
}

template<typename... Types>
struct variant_base;

template<typename... Types>
struct variant_ctor_raw_visitor {
  variant_base<Types...>& base;

  template<typename T, std::size_t I>
  PREVIEW_CONSTEXPR_AFTER_CXX17 void operator()(T&& value, in_place_index_t<I>) {
    using U = remove_cvref_t<type_sequence_element_type_t<I, type_sequence<Types...>>>;
    ::new(preview::addressof(variant_raw_get(base.union_, in_place_index<I>))) U(std::forward<T>(value));
    base.index_ = I;
  }

  template<typename T>
  constexpr void operator()(T&&, in_place_index_t<variant_npos>) {
    base.index_ = variant_npos;
  }
};

template<typename... Types>
struct variant_assign_raw_visitor {
  variant_base<Types...>& base;

  template<typename T, std::size_t I>
  PREVIEW_CONSTEXPR_AFTER_CXX17 void copy_assign_different(T& value, in_place_index_t<I>, std::true_type /* direct construct */) {
    base.reset();
    using U = remove_cvref_t<type_sequence_element_type_t<I, type_sequence<Types...>>>;
    ::new(preview::addressof(variant_raw_get(base.union_, in_place_index<I>))) U(value);
  }

  template<typename T, std::size_t I>
  PREVIEW_CONSTEXPR_AFTER_CXX17 void copy_assign_different(T& value, in_place_index_t<I>, std::false_type /* direct construct */) {
    auto temp = value;
    base.reset();
    using U = remove_cvref_t<type_sequence_element_type_t<I, type_sequence<Types...>>>;
    ::new(preview::addressof(variant_raw_get(base.union_, in_place_index<I>))) U(std::move(temp));
  }

  template<typename T, std::size_t I>
  PREVIEW_CONSTEXPR_AFTER_CXX17 void assign_different(T&& value, in_place_index_t<I>) {
    base.reset();
    using U = remove_cvref_t<type_sequence_element_type_t<I, type_sequence<Types...>>>;
    ::new(preview::addressof(variant_raw_get(base.union_, in_place_index<I>))) U(std::forward<T>(value));
  }

  template<typename T, std::size_t I>
  constexpr void assign_different(T& value, in_place_index_t<I>) {
    copy_assign_different(value, in_place_index<I>,
        disjunction<
            std::is_nothrow_copy_constructible<remove_cvref_t<T>>,
            negation< std::is_nothrow_move_constructible<remove_cvref_t<T>> >
        >{});
  }

  template<typename T, std::size_t I>
  constexpr void operator()(T&& value, in_place_index_t<I>) {
    if (base.index_ == I) {
      variant_raw_get(base.union_, in_place_index<I>) = std::forward<T>(value);
    } else {
      assign_different(std::forward<T>(value), in_place_index<I>);
      base.index_ = I;
    }
  }

  template<typename T>
  constexpr void operator()(T&&, in_place_index_t<variant_npos>) {
    base.reset();
  }
};

template<typename F, bool Const, typename... Types>
struct variant_op_visitor {
  using base_type = std::conditional_t<Const, const variant_base<Types...>, variant_base<Types...>>;
  base_type& thiz;

  template<typename T, std::size_t I>
  constexpr bool operator()(const T& y, in_place_index_t<I>) {
    return F{}(variant_raw_get(thiz.union_, in_place_index<I>), y);
  }

  template<typename T>
  constexpr bool operator()(const T&, in_place_index_t<variant_npos>) {
    // unreachable
    assert(((void)"Invalid access", false));
    return false;
  }
};

template<typename... Types>
struct variant_base {
  PREVIEW_CONSTEXPR_AFTER_CXX20 variant_base()
      : union_{}, index_{variant_npos} {}

  template<typename... U, std::size_t I>
  constexpr explicit variant_base(in_place_index_t<I>, U&&... args)
      : union_(in_place_index<I>, std::forward<U>(args)...), index_(I) {}

  constexpr void construct_from(const variant_base& other) {
    variant_raw_visit(other.index_, other.union_, variant_ctor_raw_visitor<Types...>{*this});
  }
  constexpr void construct_from(variant_base&& other)
      noexcept(conjunction<std::is_nothrow_move_constructible<Types>...>::value) {
    variant_raw_visit(other.index_, std::move(other.union_), variant_ctor_raw_visitor<Types...>{*this});
  }

  constexpr void assign_from(const variant_base& other) {
    variant_raw_visit(other.index_, other.union_, variant_assign_raw_visitor<Types...>{*this});
  }
  constexpr void assign_from(variant_base&& other)
      noexcept(conjunction<
          std::is_nothrow_move_constructible<Types>...,
          std::is_nothrow_move_assignable<Types>...
        >::value) {
    variant_raw_visit(other.index_, std::move(other.union_), variant_assign_raw_visitor<Types...>{*this});
  }

  PREVIEW_CONSTEXPR_AFTER_CXX20 void destroy_unchecked() {
    variant_raw_visit(index_, union_, [](auto& x, auto) {
        using T = remove_cvref_t<decltype(x)>;
        x.~T();
    });
  }

  template<typename Tj, typename T, std::size_t I>
  PREVIEW_CONSTEXPR_AFTER_CXX20 void assign_different(T&& t, in_place_index_t<I>, std::true_type) {
    emplace<I>(std::forward<T>(t));
  }

  template<typename Tj, typename T, std::size_t I>
  PREVIEW_CONSTEXPR_AFTER_CXX20 void assign_different(T&& t, in_place_index_t<I>, std::false_type) {
    emplace<I>(Tj(std::forward<T>(t)));
  }

  template<typename T, std::size_t I>
  PREVIEW_CONSTEXPR_AFTER_CXX20 void assign(T&& t, std::integral_constant<std::size_t, I>) {
    if (index_ == I) {
      variant_raw_get(union_, in_place_index<I>) = std::forward<T>(t);
    } else {
      using Tj = type_sequence_element_type_t<I, type_sequence<Types...>>;
      assign_different<Tj>(std::forward<T>(t), in_place_index<I>,
          disjunction< std::is_nothrow_constructible<Tj, T>, negation< std::is_nothrow_move_constructible<Tj> > >{});
    }
  }

  template<std::size_t I, typename... Args>
  PREVIEW_CONSTEXPR_AFTER_CXX20 type_sequence_element_type_t<I, type_sequence<Types...>>&
  emplace(Args&&... args) {
    reset();
    return emplace_valueless<I>(std::forward<Args>(args)...);
  }

  template<std::size_t I, typename... Args>
  PREVIEW_CONSTEXPR_AFTER_CXX20 type_sequence_element_type_t<I, type_sequence<Types...>>&
  emplace_valueless(Args&&... args) {
    using U = remove_cvref_t<type_sequence_element_type_t<I, type_sequence<Types...>>>;
    auto& elem = variant_raw_get(union_, in_place_index<I>);
    ::new(preview::addressof(elem)) U(std::forward<Args>(args)...);
    index_ = I;
    return elem;
  }

  template<std::size_t I>
  struct swapper_inner {
    variant_base& thiz;
    variant_base& other;

    // I <-> I
    template<typename T, std::size_t J, std::enable_if_t<(J != variant_npos && J == I), int> = 0>
    void operator()(T& y, in_place_index_t<J>) {
      using std::swap;
      T& x = variant_raw_get(thiz.storage(), in_place_index<I>);
      swap(x, y);
    }

    // I <-> J
    template<typename U, std::size_t J, std::enable_if_t<(J != variant_npos && J != I && I != variant_npos), int> = 0>
    void operator()(U& y, in_place_index_t<J>) {
      auto x = std::move(variant_raw_get(thiz.storage(), in_place_index<I>));
      thiz.reset(in_place_index<I>);
      thiz.template emplace_valueless<J>(std::move(y));
      other.reset(in_place_index<J>);
      other.template emplace_valueless<I>(std::move(x));
    }

    // valuelesss <-> J
    template<typename U, std::size_t J, std::enable_if_t<(J != variant_npos && I == variant_npos), int> = 0>
    void operator()(U& y, in_place_index_t<J>) {
      thiz.template emplace_valueless<J>(std::move(y));
      other.reset(in_place_index<J>);
    }

    // I <-> valueless
    template<typename U, std::size_t J, std::enable_if_t<(J == variant_npos && J != I), int> = 0>
    void operator()(U&, in_place_index_t<J>) {
      auto& x = variant_raw_get(thiz.storage(), in_place_index<I>);
      other.template emplace_valueless<I>(std::move(x));
      thiz.reset(in_place_index<I>);
    }

    // valueless <-> valueless
    template<typename U, std::size_t J, std::enable_if_t<(J == variant_npos && J == I), int> = 0>
    void operator()(U&, in_place_index_t<J>) {}
  };

  void swap_base_impl(variant_base& rhs, std::true_type /* trivial */) {
    using std::swap;
    swap(*this, rhs);
  }

  void swap_base_impl(variant_base& rhs, std::false_type /* trivial */) {
    variant_raw_visit(index_, union_, [this, &rhs](auto&, auto i) {
      constexpr std::size_t I = index_value<decltype(i)>::value;
      variant_raw_visit(rhs.index_, rhs.storage(), swapper_inner<I>{*this, rhs});
    });
  }

  void swap_base(variant_base& rhs) {
    swap_base_impl(rhs, conjunction<std::is_trivial<Types>...>{});
  }

  void reset() {
    if (index_ != variant_npos) {
      destroy_unchecked();
      index_ = variant_npos;
    }
  }

  template<std::size_t I>
  void reset(in_place_index_t<I>) {
    auto& x = variant_raw_get(union_, in_place_index<I>);
    using T = remove_cvref_t<decltype(x)>;
    x.~T();
    index_ = variant_npos;
  }

  union_wrapper<Types...>& storage() & { return union_; }
  const union_wrapper<Types...>& storage() const & { return union_; }
  union_wrapper<Types...>&& storage() && { return std::move(union_); }
  const union_wrapper<Types...>&& storage() const && { return std::move(union_); }

  union_wrapper<Types...> union_;
  std::size_t index_;
};

template<typename... Types>
struct variant_nontrivial_dtor : variant_base<Types...> {
  using base = variant_base<Types...>;
  using base::base;

  PREVIEW_CONSTEXPR_AFTER_CXX20 ~variant_nontrivial_dtor() {
    if (base::index_ != variant_npos) {
      base::destroy_unchecked();
    }
  }

  variant_nontrivial_dtor() = default;
  variant_nontrivial_dtor(const variant_nontrivial_dtor&) = default;
  variant_nontrivial_dtor(variant_nontrivial_dtor&&) = default;
  variant_nontrivial_dtor& operator=(const variant_nontrivial_dtor&) = default;
  variant_nontrivial_dtor& operator=(variant_nontrivial_dtor&&) = default;
};

template<typename... Types>
using variant_control_smf =
    std::conditional_t<
        conjunction<std::is_trivially_destructible<Types>...>::value,
        control_special<variant_base<Types...>, Types...>,
        control_special<variant_nontrivial_dtor<Types...>, Types...>
    >;

template<typename T> struct is_in_place_index : std::false_type {};
template<std::size_t I> struct is_in_place_index<in_place_index_t<I>> : std::true_type {};

// Get matching type for overload
template<std::size_t I, typename...>
struct variant_overload_t;

template<std::size_t I, typename T>
struct variant_overload_t<I, T> {
  template<typename U, std::enable_if_t<is_copy_list_initializable<T, U>::value, int> = 0>
  type_identity<T> test(T v);
};

template<std::size_t I, typename T, typename ...Ts>
struct variant_overload_t<I, T, Ts...> : variant_overload_t<I + 1, Ts...> {
  using base = variant_overload_t<I + 1, Ts...>;
  using base::test;

  template<typename U, std::enable_if_t<is_copy_list_initializable<T, U>::value, int> = 0>
  type_identity<T> test(T v);
};

template<typename...T>
using variant_overload = variant_overload_t<0, T...>;

template<typename T, typename TypeSeq, typename = void>
struct variant_has_overload : std::false_type {};

template<typename T, typename ...Types>
struct variant_has_overload<T, type_sequence<Types...>,
    void_t<decltype(std::declval<variant_overload<Types...>>().template test<T>(std::declval<T>()))>>
    : std::true_type
{
  using overload_type = typename decltype(
      std::declval<variant_overload<Types...>>().template test<T>(std::declval<T>())
  )::type;
};

template<typename T, typename TypeSeq>
using variant_overload_type = typename variant_has_overload<T, TypeSeq>::overload_type;

template<typename T, typename TypeSeq>
using variant_overload_type_index = type_sequence_type_index<variant_overload_type<T, TypeSeq>, TypeSeq>;

template<typename T, typename TypeSeq, bool = variant_has_overload<T, TypeSeq>::value /* false */>
struct variant_has_unique_overload : std::false_type {};

template<typename T, typename TypeSeq>
struct variant_has_unique_overload<T, TypeSeq, true>
    : bool_constant<type_sequence_type_count<variant_overload_type<T, TypeSeq>, TypeSeq>::value == 1> {};

template<typename T, typename TypeSeq, bool = variant_has_unique_overload<T, TypeSeq>::value>
struct variant_assign_check : std::false_type {};
template<typename T, typename TypeSeq>
struct variant_assign_check<T, TypeSeq, true> :
    conjunction<
        std::is_assignable<variant_overload_type<T, TypeSeq>&, T>,
        std::is_constructible<variant_overload_type<T, TypeSeq>, T>
    > {};

template<typename Visitor, typename Variant>
constexpr decltype(auto) visit_single(Visitor&& vis, Variant&& var);

template<typename R, typename Visitor, typename Variant>
constexpr R visit_single(Visitor&& vis, Variant&& var);

struct variant_valueless_ctor {};

} // namespace detail

template<typename... Types>
class variant : private detail::variant_control_smf<Types...> {
  using base = detail::variant_control_smf<Types...>;
  using TypeSeq = type_sequence<Types...>;

 public:
  static_assert(sizeof...(Types) > 0, "Variant must not be empty");
  static_assert(disjunction<std::is_array<Types>...>::value == false, "Type must not be an array");
  static_assert(disjunction<std::is_reference<Types>...>::value == false, "Type must not be a reference");
  static_assert(disjunction<std::is_void<Types>...>::value == false, "Type must not be a void");

  constexpr explicit variant(detail::variant_valueless_ctor) noexcept
      : base() {}

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      std::is_default_constructible<type_sequence_element_type_t<0, TypeSeq>>
  >::value, int> = 0>
  constexpr variant()
      noexcept(std::is_nothrow_default_constructible<type_sequence_element_type_t<0, TypeSeq>>::value)
      : base(in_place_index<0>) {}

  template<typename T, std::enable_if_t<conjunction<
      different_from<T, variant>,
      negation< detail::is_in_place_index<remove_cvref_t<T>> >,
      negation< is_specialization<remove_cvref_t<T>, in_place_type_t> >,
      detail::variant_has_unique_overload<T, TypeSeq>
  >::value, int> = 0>
  constexpr variant(T&& t)
      noexcept(std::is_nothrow_constructible<detail::variant_overload_type<T, TypeSeq>, T>::value)
      : base(in_place_index<detail::variant_overload_type_index<T, TypeSeq>::value>, std::forward<T>(t)) {}

  template<typename T, typename... Args, std::enable_if_t<conjunction<
      type_sequence_unique<T, TypeSeq>,
      std::is_constructible<T, Args...>
  >::value, int> = 0>
  constexpr explicit variant(in_place_type_t<T>, Args&&... args)
      : base(in_place_index<type_sequence_type_index<T, TypeSeq>::value>, std::forward<Args>(args)...) {}

  template<typename T, typename U, typename... Args, std::enable_if_t<conjunction<
      type_sequence_unique<T, TypeSeq>,
      std::is_constructible<T, std::initializer_list<U>&, Args...>
  >::value, int> = 0>
  constexpr explicit variant(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
      : base(in_place_index<TypeSeq::template index<T>>, il, std::forward<Args>(args)...) {}

  template<std::size_t I, typename... Args, std::enable_if_t<conjunction<
      bool_constant<( I < sizeof...(Types) )>,
      std::is_constructible<type_sequence_element_type_t<I, TypeSeq>, Args...>
  >::value, int> = 0>
  constexpr explicit variant(in_place_index_t<I>, Args&&... args)
      : base(in_place_index<I>, std::forward<Args>(args)...) {}

  template<std::size_t I, typename U, typename... Args, std::enable_if_t<conjunction<
      bool_constant<( I < sizeof...(Types) )>,
      std::is_constructible<type_sequence_element_type_t<I, TypeSeq>, std::initializer_list<U>&, Args...>
  >::value, int> = 0>
  constexpr explicit variant(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
      : base(in_place_index<I>, il, std::forward<Args>(args)...) {}

  template<typename T, std::enable_if_t<conjunction<
      different_from<T, variant>,
      detail::variant_assign_check<T, TypeSeq>
  >::value, int> = 0>
  constexpr variant& operator=(T&& t)
      noexcept(conjunction<
          std::is_nothrow_assignable<detail::variant_overload_type<T, TypeSeq>&, T>,
          std::is_nothrow_constructible<detail::variant_overload_type<T, TypeSeq>, T> >::value)
  {
    base::assign(std::forward<T>(t), detail::variant_overload_type_index<T, TypeSeq>{});
    return *this;
  }

  constexpr std::size_t index() const noexcept {
    return base::index_;
  }

  constexpr bool valueless_by_exception() const noexcept {
    return base::index_ == variant_npos;
  }

  template<typename T, typename... Args, std::enable_if_t<conjunction<
      std::is_constructible<T, Args...>,
      type_sequence_unique<T, TypeSeq>
  >::value, int> = 0>
  PREVIEW_CONSTEXPR_AFTER_CXX20 T& emplace(Args&&... args) {
    return emplace<TypeSeq::template index<T>>(std::forward<Args>(args)...);
  }

  template<typename T, typename U, typename... Args, std::enable_if_t<conjunction<
      std::is_constructible<T, std::initializer_list<U>&, Args...>,
      type_sequence_unique<T, TypeSeq>
  >::value, int> = 0>
  PREVIEW_CONSTEXPR_AFTER_CXX20 T& emplace(std::initializer_list<U> il, Args&&... args) {
    return emplace<TypeSeq::template index<T>>(il, std::forward<Args>(args)...);
  }

  template<std::size_t I, typename... Args, std::enable_if_t<
      std::is_constructible<variant_alternative_t<I, variant>, Args...>
  ::value, int> = 0>
  PREVIEW_CONSTEXPR_AFTER_CXX20 variant_alternative_t<I, variant>& emplace(Args&&... args) {
    return base::template emplace<I>(std::forward<Args>(args)...);
  }

  template<std::size_t I, typename T, typename... Args, std::enable_if_t<
      std::is_constructible<variant_alternative_t<I, variant>, std::initializer_list<T>&, Args...>
  ::value, int> = 0>
  PREVIEW_CONSTEXPR_AFTER_CXX20 variant_alternative_t<I, variant>& emplace(std::initializer_list<T> il, Args&&... args) {
    return base::template emplace<I>(il, std::forward<Args>(args)...);
  }

  constexpr void swap(variant& rhs)
      noexcept(conjunction<std::is_nothrow_move_constructible<Types>..., is_nothrow_swappable<Types>... >::value)
  {
    base::swap_base(rhs);
  }

  template<typename Visitor>
  constexpr decltype(auto) visit(Visitor&& vis) & {
    return detail::visit_single(std::forward<Visitor>(vis), *this);
  }

  template<typename Visitor>
  constexpr decltype(auto) visit(Visitor&& vis) const & {
    return detail::visit_single(std::forward<Visitor>(vis), *this);
  }

  template<typename Visitor>
  constexpr decltype(auto) visit(Visitor&& vis) && {
    return detail::visit_single(std::forward<Visitor>(vis), std::move(*this));
  }

  template<typename Visitor>
  constexpr decltype(auto) visit(Visitor&& vis) const && {
    return detail::visit_single(std::forward<Visitor>(vis), std::move(*this));
  }

  template<typename R, typename Visitor>
  constexpr R visit(Visitor&& vis) & {
    return detail::visit_single<R>(std::forward<Visitor>(vis), *this);
  }

  template<typename R, typename Visitor>
  constexpr R visit(Visitor&& vis) const & {
    return detail::visit_single<R>(std::forward<Visitor>(vis), *this);
  }

  template<typename R, typename Visitor>
  constexpr R visit(Visitor&& vis) && {
    return detail::visit_single<R>(std::forward<Visitor>(vis), std::move(*this));
  }

  template<typename R, typename Visitor>
  constexpr R visit(Visitor&& vis) const && {
    return detail::visit_single<R>(std::forward<Visitor>(vis), std::move(*this));
  }

  detail::variant_base<Types...>& _base() & { return static_cast<detail::variant_base<Types...>&>(*this); }
  detail::variant_base<Types...>&& _base() && { return static_cast<detail::variant_base<Types...>&&>(*this); }
  const detail::variant_base<Types...>& _base() const & { return static_cast<const detail::variant_base<Types...>&>(*this); }
  const detail::variant_base<Types...>&& _base() const && { return static_cast<const detail::variant_base<Types...>&&>(*this); }

 private:
  struct visitor_self {
    template<typename T, std::size_t I, typename Visitor>
    constexpr decltype(auto) operator()(T&& x, in_place_index_t<I>, Visitor&& vis) const {
      return preview::invoke(std::forward<Visitor>(vis), std::forward<T>(x));
    }

    template<typename Union, typename Visitor>
    constexpr decltype(auto) operator()(Union&& u, in_place_index_t<variant_npos>, Visitor&& vis) const {
      throw bad_variant_access{};
      return preview::invoke(std::forward<Visitor>(vis), std::forward<Union>(u).get());
    }
  };
};

template<std::size_t I, typename... Types>
constexpr variant_alternative_t<I, variant<Types...>>&
get(variant<Types...>& v) {
  if (v.index() != I)
    throw bad_variant_access{};
  return detail::variant_raw_get(v._base().storage(), in_place_index<I>);
}

template<std::size_t I, typename... Types>
constexpr variant_alternative_t<I, variant<Types...>>&&
get(variant<Types...>&& v) {
  if (v.index() != I)
    throw bad_variant_access{};
  return std::move(detail::variant_raw_get(v._base().storage(), in_place_index<I>));
}

template<std::size_t I, typename... Types>
constexpr const variant_alternative_t<I, variant<Types...>>&
get(const variant<Types...>& v) {
  if (v.index() != I)
    throw bad_variant_access{};
  return detail::variant_raw_get(v._base().storage(), in_place_index<I>);
}

template<std::size_t I, typename... Types>
constexpr const variant_alternative_t<I, variant<Types...>>&&
get(const variant<Types...>&& v) {
  if (v.index() != I)
    throw bad_variant_access{};
  return std::move(detail::variant_raw_get(v._base().storage(), in_place_index<I>));
}

template<typename T, typename... Types, std::enable_if_t<
    (type_sequence_type_count<T, type_sequence<Types...>>::value == 1), int> = 0>
constexpr T& get(variant<Types...>& v) {
  return get<type_sequence_type_index<T, type_sequence<Types...>>::value>(v);
}

template<typename T, typename... Types, std::enable_if_t<
    (type_sequence_type_count<T, type_sequence<Types...>>::value == 1), int> = 0>
constexpr T&& get(variant<Types...>&& v) {
  return std::move(get<type_sequence_type_index<T, type_sequence<Types...>>::value>(std::move(v)));
}

template<typename T, typename... Types, std::enable_if_t<
    (type_sequence_type_count<T, type_sequence<Types...>>::value == 1), int> = 0>
constexpr const T& get(const variant<Types...>& v) {
  return get<type_sequence_type_index<T, type_sequence<Types...>>::value>(v);
}

template<typename T, typename... Types, std::enable_if_t<
    (type_sequence_type_count<T, type_sequence<Types...>>::value == 1), int> = 0>
constexpr const T&& get(const variant<Types...>&& v) {
  return std::move(get<type_sequence_type_index<T, type_sequence<Types...>>::value>(std::move(v)));
}

template<std::size_t I, typename... Types>
constexpr std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>
get_if(variant<Types...>* pv) noexcept {
  if (pv->index() != I)
    return nullptr;
  return preview::addressof(detail::variant_raw_get(pv->_base().storage(), in_place_index<I>));
}

template<std::size_t I, typename... Types>
constexpr std::add_pointer_t<const variant_alternative_t<I, variant<Types...>>>
get_if(const variant<Types...>* pv) noexcept {
  if (pv->index() != I)
    return nullptr;
  return preview::addressof(detail::variant_raw_get(pv->_base().storage(), in_place_index<I>));
}

template<typename T, typename... Types, std::enable_if_t<
    (type_sequence_type_count<T, type_sequence<Types...>>::value == 1), int> = 0>
constexpr std::add_pointer_t<T>
get_if(variant<Types...>* pv) noexcept {
  constexpr std::size_t I = type_sequence_type_index<T, type_sequence<Types...>>::value;
  if (pv->index() != I)
    return nullptr;
  return preview::addressof(detail::variant_raw_get(pv->_base().storage(), in_place_index<I>));
}

template<typename T, typename... Types, std::enable_if_t<
    (type_sequence_type_count<T, type_sequence<Types...>>::value == 1), int> = 0>
constexpr std::add_pointer_t<const T>
get_if(const variant<Types...>* pv) noexcept {
  constexpr std::size_t I = type_sequence_type_index<T, type_sequence<Types...>>::value;
  if (pv->index() != I)
    return nullptr;
  return preview::addressof(detail::variant_raw_get(pv->_base().storage(), in_place_index<I>));
}

template<typename... Types, std::enable_if_t<conjunction<equality_comparable<Types>...>::value, int> = 0>
constexpr bool operator==(const variant<Types...>& v, const variant<Types...>& w) {
  if (v.index() != w.index())
    return false;
  if (v.valueless_by_exception())
    return true;
  using visitor = detail::variant_op_visitor<ranges::equal_to, true, Types...>;
  return detail::variant_raw_visit(w.index(), w._base().storage(), visitor{v._base()});
}

template<typename... Types, std::enable_if_t<conjunction<equality_comparable<Types>...>::value, int> = 0>
constexpr bool operator!=(const variant<Types...>& v, const variant<Types...>& w) {
  if (v.index() != w.index())
    return true;
  if (v.valueless_by_exception())
    return false;
  using visitor = detail::variant_op_visitor<ranges::not_equal_to, true, Types...>;
  return detail::variant_raw_visit(w.index(), w._base().storage(), visitor{v._base()});
}

template<typename... Types, std::enable_if_t<conjunction<totally_ordered<Types>...>::value, int> = 0>
constexpr bool operator<(const variant<Types...>& v, const variant<Types...>& w) {
  if (w.valueless_by_exception())
    return false;
  if (v.valueless_by_exception())
    return true;
  if (v.index() != w.index()) {
    return v.index() < w.index();
  }

  using visitor = detail::variant_op_visitor<ranges::less, true, Types...>;
  return detail::variant_raw_visit(w.index(), w._base().storage(), visitor{v._base()});
}

template<typename... Types, std::enable_if_t<conjunction<totally_ordered<Types>...>::value, int> = 0>
constexpr bool operator>(const variant<Types...>& v, const variant<Types...>& w) {
  if (v.valueless_by_exception())
    return false;
  if (w.valueless_by_exception())
    return true;
  if (v.index() != w.index()) {
    return v.index() > w.index();
  }

  using visitor = detail::variant_op_visitor<ranges::greater, true, Types...>;
  return detail::variant_raw_visit(w.index(), w._base().storage(), visitor{v._base()});
}

template<typename... Types, std::enable_if_t<conjunction<totally_ordered<Types>...>::value, int> = 0>
constexpr bool operator<=(const variant<Types...>& v, const variant<Types...>& w) {
  if (v.valueless_by_exception())
    return true;
  if (w.valueless_by_exception())
    return false;
  if (v.index() != w.index()) {
    return v.index() < w.index();
  }

  using visitor = detail::variant_op_visitor<ranges::less_equal, true, Types...>;
  return detail::variant_raw_visit(w.index(), w._base().storage(), visitor{v._base()});
}

template<typename... Types, std::enable_if_t<conjunction<totally_ordered<Types>...>::value, int> = 0>
constexpr bool operator>=(const variant<Types...>& v, const variant<Types...>& w) {
  if (w.valueless_by_exception())
    return true;
  if (v.valueless_by_exception())
    return false;
  if (v.index() != w.index()) {
    return v.index() > w.index();
  }

  using visitor = detail::variant_op_visitor<ranges::greater_equal, true, Types...>;
  return detail::variant_raw_visit(w.index(), w._base().storage(), visitor{v._base()});
}

template <typename... Types>
constexpr std::enable_if_t<conjunction<std::is_move_constructible<Types>..., is_swappable<Types>...>::value>
swap(variant<Types...>& lhs, variant<Types...>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}

} // namespace preview

namespace std {

using ::preview::get;
using ::preview::get_if;

} // namespace std

#endif // PREVIEW_VARIANT_VARIANT_H_
