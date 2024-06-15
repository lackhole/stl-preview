//
// Created by yonggyulee on 2/3/24.
//

#ifndef PREVIEW_RANGES_VIEWS_REVERSE_H_
#define PREVIEW_RANGES_VIEWS_REVERSE_H_

#include <iterator>
#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__ranges/range_adaptor_closure.h"
#include "preview/__ranges/subrange.h"
#include "preview/__ranges/viewable_range.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/reverse_view.h"
#include "preview/__type_traits/detail/return_category.h"
#include "preview/__type_traits/is_specialization.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

using preview::detail::return_category;

class reverse_adaptor_object : public range_adaptor_closure<reverse_adaptor_object> {
  template<typename R>
  struct is_reverse_subrange : return_category<0> {};
  template<typename I, subrange_kind K>
  struct is_reverse_subrange<subrange<std::reverse_iterator<I>, std::reverse_iterator<I>, K>>
      : return_category<2, subrange<I, I, K>> {};

  template<typename R>
  using reverse_tag = std::conditional_t<
        is_specialization<remove_cvref_t<R>, reverse_view>::value, return_category<1>,
        is_reverse_subrange<remove_cvref_t<R>>
      >;

  template<typename R>
  constexpr auto call(R&& r, return_category<1>) const {
    return std::forward<decltype(std::forward<R>(r).base())>(std::forward<R>(r).base());
  }

  template<typename R, typename I>
  constexpr auto call(R&& r, return_category<2, subrange<I, I, subrange_kind::sized>>) const {
    return subrange<I, I, subrange_kind::sized>(r.end().base(), r.begin().base(), r.size());
  }

  template<typename R, typename I>
  constexpr auto call(R&& r, return_category<2, subrange<I, I, subrange_kind::unsized>>) const {
    return subrange<I, I, subrange_kind::unsized>(r.end().base(), r.begin().base());
  }

  template<typename R>
  constexpr auto call(R&& r, return_category<0>) const {
    return reverse_view<all_t<R>>(std::forward<R>(r));
  }

 public:
  template<typename R, std::enable_if_t<viewable_range<R>::value, int> = 0>
  constexpr auto operator()(R&& r) const {
    return call(std::forward<R>(r), reverse_tag<R>{});
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::reverse_adaptor_object reverse{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_REVERSE_H_
