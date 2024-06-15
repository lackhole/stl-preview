//
// Created by yonggyulee on 2024/01/22.
//

#ifndef PREVIEW_RANGES_VIEWS_ENUMERATE_H_
#define PREVIEW_RANGES_VIEWS_ENUMERATE_H_

#include <type_traits>
#include <tuple>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__ranges/range_adaptor_closure.h"
#include "preview/__ranges/viewable_range.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/enumerate_view.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

template<template<typename...> class Tuple>
class enumerate_adaptor_closure : public range_adaptor_closure<enumerate_adaptor_closure<Tuple>> {
  public:
  enumerate_adaptor_closure() = default;

  template<typename R, std::enable_if_t<viewable_range<R>::value, int> = 0>
  constexpr auto operator()(R&& r) const {
    return enumerate_view<all_t<R>, Tuple>(std::forward<R>(r));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::enumerate_adaptor_closure<std::tuple> enumerate{};

PREVIEW_INLINE_VARIABLE constexpr detail::enumerate_adaptor_closure<std::pair> enumerate_pair{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_ENUMERATE_H_
