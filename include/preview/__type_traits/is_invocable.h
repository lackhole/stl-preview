//
// Created by yonggyulee on 2023/12/29.
//

#ifndef PREVIEW_TYPE_TRAITS_IS_INVOCABLE_H_
#define PREVIEW_TYPE_TRAITS_IS_INVOCABLE_H_

#include <type_traits>

#include "preview/core.h"
#include "preview/__type_traits/detail/invoke.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/is_complete.h"
#include "preview/__type_traits/is_unbounded_array.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/type_identity.h"

namespace preview {
namespace detail {

struct not_invocable {};

template<typename T>
struct satisfies_invocable_type : disjunction<is_complete<T>, std::is_void<T>, is_unbounded_array<T>> {};

template<typename R, typename F, typename ...Args>
struct is_invocable_r_impl {
 // TODO: handle requirements
 //  static_assert(satisfies_invocable_type<R>::value, "invocable: Return type does not meet the requirements");
 //  static_assert(satisfies_invocable_type<F>::value, "invocable: Callable type does not meet the requirements");
 //  static_assert(conjunction<satisfies_invocable_type<Args>...>::value,
 //                "invocable: Argument types do not meet the requirements");
 private:
  template<typename F2, typename ...Args2>
  static auto test(int)
  noexcept(noexcept(detail::INVOKE(std::declval<F2>(), std::declval<Args2>()...)))
       -> decltype(detail::INVOKE(std::declval<F2>(), std::declval<Args2>()...));
  template<typename F2, typename ...Args2>
  static auto test(...) -> not_invocable;

 public:
  using test_return_type = decltype(test<F, Args...>(0));
  using convertible = disjunction<std::is_void<R>, std::is_convertible<test_return_type, R>>;
  using invocable = conjunction<negation<std::is_same<test_return_type, not_invocable>>, convertible>;
  using nothrow_invocable = conjunction<invocable, bool_constant<noexcept(test<F, Args...>(0))>>;
};

} // namespace detail

template<typename F, typename ...Args>
struct invoke_result
    : std::conditional_t<
          detail::is_invocable_r_impl<void, F, Args...>::invocable::value,
              type_identity<typename detail::is_invocable_r_impl<void, F, Args...>::test_return_type>,
          detail::not_invocable> {};

template<typename F, typename ...Args>
using invoke_result_t = typename invoke_result<F, Args...>::type;

template<typename F, typename ...Args>
struct is_invocable : detail::is_invocable_r_impl<void, F, Args...>::invocable {};

template<typename R, typename F, typename ...Args>
struct is_invocable_r : detail::is_invocable_r_impl<R, F, Args...>::invocable {};

template<typename F, typename ...Args>
struct is_nothrow_invocable : detail::is_invocable_r_impl<void, F, Args...>::nothrow_invocable {};

template<typename R, typename F, typename ...Args>
struct is_nothrow_invocable_r : detail::is_invocable_r_impl<R, F, Args...>::nothrow_invocable {};

template<typename F, typename... Args>
PREVIEW_INLINE_VARIABLE constexpr bool is_invocable_v = is_invocable<F, Args...>::value;

template<typename F, typename... Args>
PREVIEW_INLINE_VARIABLE constexpr bool is_invocable_r_v = is_invocable_r<F, Args...>::value;

template<typename F, typename... Args>
PREVIEW_INLINE_VARIABLE constexpr bool is_nothrow_invocable_v = is_nothrow_invocable<F, Args...>::value;

template<typename F, typename... Args>
PREVIEW_INLINE_VARIABLE constexpr bool is_nothrow_invocable_r_v = is_nothrow_invocable_r<F, Args...>::value;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_IS_INVOCABLE_H_
