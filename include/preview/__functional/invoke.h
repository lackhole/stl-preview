# /*
#  * Created by YongGyu Lee on 2020/12/08.
#  */
#
#ifndef PREVIEW_FUNCTIONAL_INVOKE_H_
#define PREVIEW_FUNCTIONAL_INVOKE_H_
#
# include <type_traits>
# include <utility>
#
# include "preview/__type_traits/detail/invoke.h"
# include "preview/__type_traits/is_invocable.h"

namespace preview {
namespace detail {

template<typename R, typename F, typename... Args>
constexpr void invoke_r_impl(std::true_type /* is_void */, F&& f, Args&&... args);

template<typename R, typename F, typename... Args>
constexpr R invoke_r_impl(std::false_type /* is_void */, F&& f, Args&&... args);

} // namespace detail

template<typename F, typename ...Args>
constexpr invoke_result_t<F, Args...>
invoke(F&& f, Args&&... args)
noexcept(is_nothrow_invocable<F, Args...>::value)
{
  return detail::INVOKE(std::forward<F>(f), std::forward<Args>(args)...);
}

template<typename R, typename F, typename ...Args>
constexpr std::enable_if_t<is_invocable_r<R, F, Args...>::value, R>
invoke_r(F&& f, Args&&... args)
noexcept(is_nothrow_invocable_r<R, F, Args...>::value)
{
  return preview::detail::invoke_r_impl<R>(std::is_void<R>{}, std::forward<F>(f), std::forward<Args>(args)...);
}

namespace detail {

template<typename R, typename F, typename... Args>
constexpr void invoke_r_impl(std::true_type /* is_void */, F&& f, Args&&... args) {
  static_cast<void>(detail::INVOKE(std::forward<F>(f), std::forward<Args>(args)...));
}

template<typename R, typename F, typename... Args>
constexpr R invoke_r_impl(std::false_type /* is_void */, F&& f, Args&&... args) {
  return detail::INVOKE(std::forward<F>(f), std::forward<Args>(args)...);
}

} // namespace detail
} // namespace preview

#endif // PREVIEW_FUNCTIONAL_INVOKE_H_
