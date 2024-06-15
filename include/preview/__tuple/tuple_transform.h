//
// Created by YongGyu Lee on 2/20/24.
//

#ifndef PREVIEW_TUPLE_TUPLE_TRANSFORM_H_
#define PREVIEW_TUPLE_TUPLE_TRANSFORM_H_

#include <tuple>
#include <utility>

#include "preview/__functional/invoke.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace detail {

template<typename Tuple, typename F, std::size_t... I>
constexpr auto tuple_transform_impl(Tuple&& t, F&& f, std::index_sequence<I...>)
    noexcept(conjunction<
        bool_constant<noexcept(preview::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))))>...
    >::value)
{
  return std::tuple<decltype(preview::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))))...>(
      preview::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t)))...);
}

} // namespace detail

template<typename Tuple, typename F>
constexpr auto tuple_transform(Tuple&& t, F&& f)
    noexcept(noexcept(
        detail::tuple_transform_impl(
            std::forward<Tuple>(t),
            std::forward<F>(f),
            std::make_index_sequence<std::tuple_size<remove_cvref_t<Tuple>>::value>{}
        )
    ))
{
  return detail::tuple_transform_impl(
      std::forward<Tuple>(t), std::forward<F>(f),
      std::make_index_sequence<std::tuple_size<remove_cvref_t<Tuple>>::value>{});
}

} // namespace preview

#endif // PREVIEW_TUPLE_TUPLE_TRANSFORM_H_
