//
// Created by yonggyulee on 2024. 8. 4.
//

#ifndef PREVIEW_FUNCTIONAL_WRAP_FUNCTOR_H_
#define PREVIEW_FUNCTIONAL_WRAP_FUNCTOR_H_

#include <functional>
#include <type_traits>

#include "preview/__functional/is_reference_wrapper.h"
#include "preview/__functional/reference_wrapper.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace detail {

template<typename F>
constexpr F wrap_functor_impl(F& f, std::true_type) {
  return f;
}

template<typename F>
constexpr auto wrap_functor_impl(F& f, std::false_type) {
  // std::ref will result in ambiguous call in before Android NDK 22
#if PREVIEW_CONFORM_CXX20_STANDARD
  return std::reference_wrapper<F>(f);
#else
  return preview::reference_wrapper<F>(f);
#endif
}

} // namespace detail

// Wrap functor to pass to function arguments
template<typename F>
constexpr auto wrap_functor(F& f) {
  using FT = remove_cvref_t<F>;
  static_assert(!is_reference_wrapper<FT>::value, "Invalid type");
  using cheap_trivial_copy = conjunction<
      std::is_trivially_copy_constructible<FT>,
      bool_constant<(sizeof(FT) <= (sizeof(void*)) * 2)> >;
  return preview::detail::wrap_functor_impl(f, cheap_trivial_copy{});
}

template<typename F>
constexpr auto wrap_functor(preview::reference_wrapper<F> f) {
  return f;
}

template<typename F>
constexpr auto wrap_functor(std::reference_wrapper<F> f) {
  return f;
}

} // namespace preview

#endif // PREVIEW_FUNCTIONAL_WRAP_FUNCTOR_H_
