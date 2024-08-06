//
// Created by yonggyulee on 2024. 8. 4.
//

#ifndef PREVIEW_FUNCTIONAL_WRAP_FUNCTOR_H_
#define PREVIEW_FUNCTIONAL_WRAP_FUNCTOR_H_

#include <functional>
#include <type_traits>

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
  return std::ref(f);
}

} // namespace detail

// Wrap functor to pass to function arguments
template<typename F>
constexpr auto wrap_functor(F& f) {
  using FT = remove_cvref_t<F>;
  using tag_t = conjunction<std::is_trivially_copy_constructible<FT>, bool_constant<(sizeof(FT) <= 64)>>;
  return preview::detail::wrap_functor_impl(f, tag_t{});
}

} // namespace preview

#endif // PREVIEW_FUNCTIONAL_WRAP_FUNCTOR_H_
