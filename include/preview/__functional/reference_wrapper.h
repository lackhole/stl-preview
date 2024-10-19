//
// Created by yonggyulee on 2024. 10. 14.
//

#ifndef PREVIEW_FUNCTIONAL_REFERENCE_WRAPPER_H_
#define PREVIEW_FUNCTIONAL_REFERENCE_WRAPPER_H_

#include <functional>
#include <type_traits>
#include <utility>

#include "preview/config.h"
#include "preview/__core/std_version.h"
#include "preview/__compare/synth_three_way.h"
#include "preview/__functional/is_reference_wrapper.h"
#include "preview/__memory/addressof.h"
#include "preview/__type_traits/detail/invoke.h"
#include "preview/__type_traits/basic_common_reference.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/common_reference.h"
#include "preview/__type_traits/is_complete.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__utility/in_place.h"
#include "preview/__utility/cxx20_rel_ops.h"

#if PREVIEW_CONFORM_CXX20_STANDARD
#include <concepts>
#endif

namespace preview {
namespace detail {

struct can_bind_to_reference_helper {
  template<typename T>
  static void FUN(T&) noexcept;

#if defined(_MSC_VER) && _MSC_VER < 1920
  template<typename T, std::enable_if_t<always_false<T>::value, int> = 0>
  static auto FUN(T&&) -> void;
#else
  template<typename T>
  static void FUN(T&&) = delete;
#endif

  template<typename U, typename T>
  static auto test(int) -> decltype(FUN<T>(std::declval<U&>()), std::true_type{});
  template<typename U, typename T>
  static auto test(...) -> std::false_type;

  template<typename U, typename T>
  static auto test_noexcept(int) -> decltype(FUN<T>(std::declval<U&>()), bool_constant<noexcept(FUN<T>(std::declval<U&>()))>{});
  template<typename U, typename T>
  static auto test_noexcept(...) -> std::false_type;
};

template<typename U, typename T>
struct can_bind_to_reference : decltype(can_bind_to_reference_helper::test<U, T>(0)) {};
template<typename U, typename T>
struct can_bind_to_reference_noexcept : bool_constant<noexcept(can_bind_to_reference_helper::test_noexcept<U, T>(0))> {};

} // namespace detail

template<typename T>
class reference_wrapper
#if PREVIEW_CONFORM_CXX20_STANDARD
 : public std::reference_wrapper<T>
#endif
{

#if PREVIEW_CONFORM_CXX20_STANDARD
  using base = std::reference_wrapper<T>;
#else
  constexpr reference_wrapper(in_place_t, T& ref)
      : ptr_(preview::addressof(ref)) {}

  T* ptr_;
#endif

 public:
  using type = T;

#if PREVIEW_CONFORM_CXX20_STANDARD
  using base::base;
  using base::get;
  using base::operator type &;
  using base::operator();
#else

  template<typename U, std::enable_if_t<conjunction<
      detail::can_bind_to_reference<U, T>,
      negation< std::is_same<remove_cvref_t<U>, preview::reference_wrapper<T>> >,
      negation< std::is_same<remove_cvref_t<U>,     std::reference_wrapper<T>> >
  >::value, int> = 0>
  constexpr reference_wrapper(U&& u)
      noexcept(detail::can_bind_to_reference_noexcept<U, T>::value)
      : reference_wrapper(in_place, std::forward<U>(u)) {}

  constexpr reference_wrapper(const reference_wrapper& x) noexcept
      : ptr_(x.ptr_) {}
  constexpr reference_wrapper& operator=(const reference_wrapper& x) noexcept = default;

  constexpr reference_wrapper(std::reference_wrapper<T> x) noexcept
      : reference_wrapper(in_place, x.get()) {}
  constexpr reference_wrapper& operator=(std::reference_wrapper<T> x) noexcept {
    ptr_ = preview::addressof(x.get());
  }

  constexpr operator T& () const noexcept {
    return *ptr_;
  }

  constexpr T& get() const noexcept {
    return *ptr_;
  }

  template<typename... ArgTypes>
  constexpr invoke_result_t<T&, ArgTypes...>
  operator()(ArgTypes&&... args) const noexcept(is_nothrow_invocable<T&, ArgTypes...>::value) {
    static_assert(is_complete_v<T>, "T must be complete type");
    return preview::detail::INVOKE(get(), std::forward<ArgTypes>(args)...);
  }
#endif

  // Not a standard function! Use this function on deduced context is needed
  std::reference_wrapper<T> to_std() {
    return get();
  }

  // Not a standard function! This is required for std::invoke to handle reference_wrapper correctly.
  constexpr T& operator*() const noexcept {
    return get();
  }

  template<typename U = T, std::enable_if_t<rel_ops::is_equality_comparable<U&, U&>::value, int> = 0>
  friend constexpr bool operator==(reference_wrapper x, reference_wrapper y) {
    return x.get() == y.get();
  }
#if !PREVIEW_CONFORM_CXX20_STANDARD
  template<typename U = T, std::enable_if_t<rel_ops::is_equality_comparable<U&, U&>::value, int> = 0>
  friend constexpr bool operator!=(reference_wrapper x, reference_wrapper y) {
    return !(x == y);
  }
#endif

  template<typename U = T, std::enable_if_t<rel_ops::is_equality_comparable<U&, const U&>::value, int> = 0>
  friend constexpr bool operator==(reference_wrapper x, const T& y) {
    return x.get() == y;
  }
#if !PREVIEW_CONFORM_CXX20_STANDARD
  template<typename U = T, std::enable_if_t<rel_ops::is_equality_comparable<U&, const U&>::value, int> = 0>
  friend constexpr bool operator==(const T& y, reference_wrapper x) {
    return x == y;
  }
  template<typename U = T, std::enable_if_t<rel_ops::is_equality_comparable<U&, const U&>::value, int> = 0>
  friend constexpr bool operator!=(reference_wrapper x, const T& y) {
    return !(x == y);
  }
  template<typename U = T, std::enable_if_t<rel_ops::is_equality_comparable<U&, const U&>::value, int> = 0>
  friend constexpr bool operator!=(const T& y, reference_wrapper x) {
    return !(x == y);
  }
#endif

  // T  ==  const T
  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, rel_ops::is_equality_comparable<U&, const U&>>, int> = 0>
  friend constexpr bool operator==(reference_wrapper x, reference_wrapper<const T> y) {
    return x.get() == y.get();
  }
  // const T  ==  T
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, rel_ops::is_equality_comparable<std::remove_const_t<U>&, U&>>, int> = 0>
  friend constexpr bool operator==(reference_wrapper x, reference_wrapper<std::remove_const_t<T>> y) {
    return y.get() == x.get();
  }
#if !PREVIEW_CONFORM_CXX20_STANDARD
  // T  !=  const T
  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, rel_ops::is_equality_comparable<U&, const U&>>, int> = 0>
  friend constexpr bool operator!=(reference_wrapper x, reference_wrapper<const T> y) {
    return !(x == y);
  }
  // const T  !=  T
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, rel_ops::is_equality_comparable<std::remove_const_t<U>&, U&>>, int> = 0>
  friend constexpr bool operator!=(reference_wrapper x, reference_wrapper<std::remove_const_t<T>> y) {
    return !(y == x);
  }
#endif

  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, U&>::value, int> = 0>
  friend constexpr bool operator<(reference_wrapper x, reference_wrapper y) {
    return x.get() < y.get();
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, U&>::value, int> = 0>
  friend constexpr bool operator<=(reference_wrapper x, reference_wrapper y) {
    return !(y < x);
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, U&>::value, int> = 0>
  friend constexpr bool operator>(reference_wrapper x, reference_wrapper y) {
    return y < x;
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, U&>::value, int> = 0>
  friend constexpr bool operator>=(reference_wrapper x, reference_wrapper y) {
    return !(x < y);
  }

  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, const U&>::value, int> = 0>
  friend constexpr bool operator<(reference_wrapper x, const T& y) {
    return x.get() < y;
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, const U&>::value, int> = 0>
  friend constexpr bool operator<=(reference_wrapper x, const T& y) {
    return !(y < x);
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, const U&>::value, int> = 0>
  friend constexpr bool operator>(reference_wrapper x, const T& y) {
    return y < x;
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, const U&>::value, int> = 0>
  friend constexpr bool operator>=(reference_wrapper x, const T& y) {
    return !(x < y);
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, const U&>::value, int> = 0>
  friend constexpr bool operator<(const T& x, reference_wrapper y) {
    return x < y.get();
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, const U&>::value, int> = 0>
  friend constexpr bool operator<=(const T& x, reference_wrapper y) {
    return !(y < x);
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, const U&>::value, int> = 0>
  friend constexpr bool operator>(const T& x, reference_wrapper y) {
    return y < x;
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, const U&>::value, int> = 0>
  friend constexpr bool operator>=(const T& x, reference_wrapper y) {
    return !(x < y);
  }

  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, detail::synth_three_way_possible<U&, const U&>>, int> = 0>
  friend constexpr bool operator<(reference_wrapper x, reference_wrapper<const T> y) {
    return x.get() < y.get();
  }
  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, detail::synth_three_way_possible<U&, const U&>>, int> = 0>
  friend constexpr bool operator<=(reference_wrapper x, reference_wrapper<const T> y) {
    return !(y < x);
  }
  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, detail::synth_three_way_possible<U&, const U&>>, int> = 0>
  friend constexpr bool operator>(reference_wrapper x, reference_wrapper<const T> y) {
    return y < x;
  }
  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, detail::synth_three_way_possible<U&, const U&>>, int> = 0>
  friend constexpr bool operator>=(reference_wrapper x, reference_wrapper<const T> y) {
    return !(x < y);
  }
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, detail::synth_three_way_possible<U&, std::remove_const_t<U>&>>, int> = 0>
  friend constexpr bool operator<(reference_wrapper x, reference_wrapper<std::remove_const_t<T>> y) {
    return x.get() < y.get();
  }
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, detail::synth_three_way_possible<U&, std::remove_const_t<U>&>>, int> = 0>
  friend constexpr bool operator<=(reference_wrapper x, reference_wrapper<std::remove_const_t<T>> y) {
    return !(y < x);
  }
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, detail::synth_three_way_possible<U&, std::remove_const_t<U>&>>, int> = 0>
  friend constexpr bool operator>(reference_wrapper x, reference_wrapper<std::remove_const_t<T>> y) {
    return y < x;
  }
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, detail::synth_three_way_possible<U&, std::remove_const_t<U>&>>, int> = 0>
  friend constexpr bool operator>=(reference_wrapper x, reference_wrapper<std::remove_const_t<T>> y) {
    return !(x < y);
  }


  // Comparison with std::reference_wrapper<T>
  template<typename U = T, std::enable_if_t<rel_ops::is_equality_comparable<U&, U&>::value, int> = 0>
  friend constexpr bool operator==(reference_wrapper x, std::reference_wrapper<T> y) {
    return x.get() == y.get();
  }
#if !PREVIEW_CONFORM_CXX20_STANDARD
  template<typename U = T, std::enable_if_t<rel_ops::is_equality_comparable<U&, U&>::value, int> = 0>
  friend constexpr bool operator==(std::reference_wrapper<T> x, reference_wrapper y) {
    return x.get() == y.get();
  }
  template<typename U = T, std::enable_if_t<rel_ops::is_equality_comparable<U&, U&>::value, int> = 0>
  friend constexpr bool operator!=(reference_wrapper x, std::reference_wrapper<T> y) {
    return !(x == y);
  }
  template<typename U = T, std::enable_if_t<rel_ops::is_equality_comparable<U&, U&>::value, int> = 0>
  friend constexpr bool operator!=(std::reference_wrapper<T> x, reference_wrapper y) {
    return !(x == y);
  }
#endif

  // Comparison with std::reference_wrapper<const T>
  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, rel_ops::is_equality_comparable<U&, const U&>>, int> = 0>
  friend constexpr bool operator==(reference_wrapper x, std::reference_wrapper<const T> y) {
    return x.get() == y.get();
  }
  // const T  ==  T
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, rel_ops::is_equality_comparable<std::remove_const_t<U>&, U&>>, int> = 0>
  friend constexpr bool operator==(reference_wrapper x, std::reference_wrapper<std::remove_const_t<T>> y) {
    return y.get() == x.get();
  }
#if !PREVIEW_CONFORM_CXX20_STANDARD
  // T  !=  const T
  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, rel_ops::is_equality_comparable<U&, const U&>>, int> = 0>
  friend constexpr bool operator!=(reference_wrapper x, std::reference_wrapper<const T> y) {
    return !(x == y);
  }
  // const T  !=  T
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, rel_ops::is_equality_comparable<std::remove_const_t<U>&, U&>>, int> = 0>
  friend constexpr bool operator!=(reference_wrapper x, std::reference_wrapper<std::remove_const_t<T>> y) {
    return !(y == x);
  }
#endif

  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, rel_ops::is_equality_comparable<U&, const U&>>, int> = 0>
  friend constexpr bool operator==(std::reference_wrapper<const T> x, reference_wrapper y) {
    return y.get() == x;
  }
  // const T  ==  T
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, rel_ops::is_equality_comparable<std::remove_const_t<U>&, U&>>, int> = 0>
  friend constexpr bool operator==(std::reference_wrapper<std::remove_const_t<T>> x, reference_wrapper y) {
    return x.get() == y.get();
  }
#if !PREVIEW_CONFORM_CXX20_STANDARD
  // T  !=  const T
  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, rel_ops::is_equality_comparable<U&, const U&>>, int> = 0>
  friend constexpr bool operator!=(std::reference_wrapper<const T> x, reference_wrapper y) {
    return !(y == x);
  }
  // const T  !=  T
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, rel_ops::is_equality_comparable<std::remove_const_t<U>&, U&>>, int> = 0>
  friend constexpr bool operator!=(std::reference_wrapper<std::remove_const_t<T>> x, reference_wrapper y) {
    return !(y == x);
  }
#endif


  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, U&>::value, int> = 0>
  friend constexpr bool operator<(reference_wrapper x, std::reference_wrapper<T> y) {
    return x.get() < y.get();
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, U&>::value, int> = 0>
  friend constexpr bool operator<(std::reference_wrapper<T> x, reference_wrapper y) {
    return x.get() < y.get();
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, U&>::value, int> = 0>
  friend constexpr bool operator<=(reference_wrapper x, std::reference_wrapper<T> y) {
    return !(y < x);
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, U&>::value, int> = 0>
  friend constexpr bool operator<=(std::reference_wrapper<T> x, reference_wrapper y) {
    return !(y < x);
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, U&>::value, int> = 0>
  friend constexpr bool operator>(reference_wrapper x, std::reference_wrapper<T> y) {
    return y < x;
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, U&>::value, int> = 0>
  friend constexpr bool operator>(std::reference_wrapper<T> x, reference_wrapper y) {
    return y < x;
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, U&>::value, int> = 0>
  friend constexpr bool operator>=(reference_wrapper x, std::reference_wrapper<T> y) {
    return !(x < y);
  }
  template<typename U = T, std::enable_if_t<detail::synth_three_way_possible<U&, U&>::value, int> = 0>
  friend constexpr bool operator>=(std::reference_wrapper<T> x, reference_wrapper y) {
    return !(x < y);
  }

  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, detail::synth_three_way_possible<U&, const U&>>, int> = 0>
  friend constexpr bool operator<(reference_wrapper x, std::reference_wrapper<const T> y) {
    return x.get() < y.get();
  }
  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, detail::synth_three_way_possible<U&, const U&>>, int> = 0>
  friend constexpr bool operator<=(reference_wrapper x, std::reference_wrapper<const T> y) {
    return !(y < x);
  }
  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, detail::synth_three_way_possible<U&, const U&>>, int> = 0>
  friend constexpr bool operator>(reference_wrapper x, std::reference_wrapper<const T> y) {
    return y < x;
  }
  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, detail::synth_three_way_possible<U&, const U&>>, int> = 0>
  friend constexpr bool operator>=(reference_wrapper x, std::reference_wrapper<const T> y) {
    return !(x < y);
  }
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, detail::synth_three_way_possible<U&, std::remove_const_t<U>&>>, int> = 0>
  friend constexpr bool operator<(reference_wrapper x, std::reference_wrapper<std::remove_const_t<T>> y) {
    return x.get() < y.get();
  }
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, detail::synth_three_way_possible<U&, std::remove_const_t<U>&>>, int> = 0>
  friend constexpr bool operator<=(reference_wrapper x, std::reference_wrapper<std::remove_const_t<T>> y) {
    return !(y < x);
  }
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, detail::synth_three_way_possible<U&, std::remove_const_t<U>&>>, int> = 0>
  friend constexpr bool operator>(reference_wrapper x, std::reference_wrapper<std::remove_const_t<T>> y) {
    return y < x;
  }
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, detail::synth_three_way_possible<U&, std::remove_const_t<U>&>>, int> = 0>
  friend constexpr bool operator>=(reference_wrapper x, std::reference_wrapper<std::remove_const_t<T>> y) {
    return !(x < y);
  }

  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, detail::synth_three_way_possible<const U&, U&>>, int> = 0>
  friend constexpr bool operator<(std::reference_wrapper<const T> x, reference_wrapper y) {
    return x.get() < y.get();
  }
  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, detail::synth_three_way_possible<const U&, U&>>, int> = 0>
  friend constexpr bool operator<=(std::reference_wrapper<const T> x, reference_wrapper y) {
    return !(y < x);
  }
  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, detail::synth_three_way_possible<const U&, U&>>, int> = 0>
  friend constexpr bool operator>(std::reference_wrapper<const T> x, reference_wrapper y) {
    return y < x;
  }
  template<typename U = T, std::enable_if_t<conjunction_v<negation<std::is_const<U>>, detail::synth_three_way_possible<const U&, U&>>, int> = 0>
  friend constexpr bool operator>=(std::reference_wrapper<const T> x, reference_wrapper y) {
    return !(x < y);
  }
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, detail::synth_three_way_possible<std::remove_const_t<U>&, U&>>, int> = 0>
  friend constexpr bool operator<(std::reference_wrapper<std::remove_const_t<T>> x, reference_wrapper y) {
    return x.get() < y.get();
  }
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, detail::synth_three_way_possible<std::remove_const_t<U>&, U&>>, int> = 0>
  friend constexpr bool operator<=(std::reference_wrapper<std::remove_const_t<T>> x, reference_wrapper y) {
    return !(y < x);
  }
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, detail::synth_three_way_possible<std::remove_const_t<U>&, U&>>, int> = 0>
  friend constexpr bool operator>(std::reference_wrapper<std::remove_const_t<T>> x, reference_wrapper y) {
    return y < x;
  }
  template<typename U = T, std::enable_if_t<conjunction_v<std::is_const<U>, detail::synth_three_way_possible<std::remove_const_t<U>&, U&>>, int> = 0>
  friend constexpr bool operator>=(std::reference_wrapper<std::remove_const_t<T>> x, reference_wrapper y) {
    return !(x < y);
  }
};

#if PREVIEW_CXX_VERSION >= 17

template< typename T >
reference_wrapper( T& ) -> reference_wrapper<T>;

#endif

template<typename T>
constexpr reference_wrapper<T> ref(T& t) noexcept {
  return reference_wrapper<T>(t);
}

template<typename T>
constexpr reference_wrapper<T> ref(reference_wrapper<T> t) noexcept {
  return t;
}

template<typename T>
constexpr reference_wrapper<T> ref(std::reference_wrapper<T> t) noexcept {
  return reference_wrapper<T>(t.get());
}

template<typename T>
constexpr reference_wrapper<const T> cref(const T& t) noexcept {
  return reference_wrapper<const T>(t);
}

template<typename T>
constexpr reference_wrapper<const T> cref(reference_wrapper<T> t) noexcept {
  return t;
}

template<typename T>
constexpr reference_wrapper<const T> cref(std::reference_wrapper<T> t) noexcept {
  return reference_wrapper<const T>(t.get());
}

// TODO: Specialize std::basic_common_reference for preview::reference_wrapper

template<typename T>
struct is_reference_wrapper<reference_wrapper<T>> : std::true_type {};

namespace detail {

#if PREVIEW_CONFORM_CXX20_STANDARD

template<typename R, typename T, typename RQ, typename TQ>
concept ref_wrap_common_reference_exists_with =
    is_specialization_v<R, preview::reference_wrapper> &&
    requires { typename preview::common_reference_t<typename R::type&, TQ>; } &&
    std::convertible_to<RQ, preview::common_reference_t<typename R::type&, TQ>>;

#endif

namespace preview_refwrap_to_std_refwrap {

template<typename T>
std::reference_wrapper<T> ref(preview::reference_wrapper<T> x) {
  return std::reference_wrapper<T>(x.get());
}

template<typename T>
constexpr std::reference_wrapper<const T> cref(preview::reference_wrapper<T> t) noexcept {
  return std::reference_wrapper<const T>(t.get());
}

} // namespace preview_refwrap_to_std_refwrap
} // namespace detail
} // namespace preview

namespace std {

using preview::detail::preview_refwrap_to_std_refwrap::ref;
using preview::detail::preview_refwrap_to_std_refwrap::cref;

} // namespace std

#if PREVIEW_CONFORM_CXX20_STANDARD

// Single defition to prevent some old compilers complaining duplicated difinition

template<class T, class U, template<class> class TQual, template<class> class UQual>
requires (
    (preview::detail::ref_wrap_common_reference_exists_with<T, U, TQual<T>, UQual<U>> &&
     !preview::detail::ref_wrap_common_reference_exists_with<U, T, UQual<U>, TQual<T>>) ||
    (preview::detail::ref_wrap_common_reference_exists_with<U, T, UQual<U>, TQual<T>> &&
     !preview::detail::ref_wrap_common_reference_exists_with<T, U, TQual<T>, UQual<U>>)
)
struct std::basic_common_reference<T, U, TQual, UQual> {
  using type = typename preview::detail::bcr_specialization::basic_common_reference_base<
      T, U, TQual, UQual, preview::detail::bcr_specialization::kReferenceWrapper>::type;
};

#endif

#endif // PREVIEW_FUNCTIONAL_REFERENCE_WRAPPER_H_
