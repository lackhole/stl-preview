#ifndef PREVIEW_FUNCTIONAL_BIND_BACK_H_
#define PREVIEW_FUNCTIONAL_BIND_BACK_H_

#include <cstddef>
#include <type_traits>
#include <utility>

#include "preview/__functional/detail/bind_base.h"
#include "preview/__functional/invoke.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/copy_cvref.h"
#include "preview/__type_traits/is_invocable.h"

namespace preview {
namespace detail {

template<typename F, typename... Args>
class bind_back_object : public bind_object_base<bind_back_object<F, Args...>, F, Args...> {
  using base = bind_object_base<bind_back_object<F, Args...>, F, Args...>;
  friend base;

  template<typename Self, typename... U>
  struct bind_invoke_result
      : invoke_result<copy_cvref_t<Self&&, F>, U&&..., copy_cvref_t<Self&&, Args>...> {};

  template<typename Self, typename... U>
  struct bind_nothrow_invocable
      : is_nothrow_invocable<copy_cvref_t<Self&&, F>, U&&..., copy_cvref_t<Self&&, Args>...> {};

  template<std::size_t...I, typename Self, typename... U>
  static constexpr typename bind_invoke_result<Self&&, U&&...>::type
  call(Self&& self, std::index_sequence<I...>, U&&... args) noexcept(bind_nothrow_invocable<Self&&, U&&...>::value) {
    return preview::invoke(
        std::forward<Self>(self).func_,
        std::forward<U>(args)...,
        std::get<I>(std::forward<Self>(self).args_)...
    );
  }

 public:
  using base::base;
  using base::operator();
};

} // namespace detail

template<typename F, typename... Args, std::enable_if_t<conjunction<
    std::is_constructible<std::decay_t<F>, F>,
    std::is_move_constructible<std::decay_t<F>>,
    std::is_constructible<std::decay_t<Args>, Args>...,
    std::is_move_constructible<std::decay_t<Args>>...
>::value, int> = 0>
constexpr auto bind_back(F&& f, Args&&... args) {
  return detail::bind_back_object<std::decay_t<F>, Args&&...>{
      detail::bind_object_ctor_tag{},
      std::forward<F>(f),
      std::forward<Args>(args)...
  };
}

} // namespace preview

#endif // PREVIEW_FUNCTIONAL_BIND_BACK_H_
