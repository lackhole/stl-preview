//
// Created by yonggyulee on 2023/12/29.
//

#ifndef PREVIEW_RANGES_VIEWS_ALL_H_
#define PREVIEW_RANGES_VIEWS_ALL_H_

#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__ranges/owning_view.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/range_adaptor_closure.h"
#include "preview/__ranges/ref_view.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/viewable_range.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/detail/return_category.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

using preview::detail::return_category;

class all_adaptor_closure : public range_adaptor_closure<all_adaptor_closure> {
  template<typename T>
  using return_category_type =
      conditional_t<
          view<std::decay_t<T>>, return_category<1, std::decay_t<T>>,
          conjunction< // ref_view{T}
              std::is_lvalue_reference<T>,
              std::is_convertible<T, std::add_lvalue_reference_t<std::remove_reference_t<T>>>,
              range<std::remove_reference_t<T>>,
              std::is_object<std::remove_reference_t<T>>
          >,
          return_category<2, ref_view<std::remove_reference_t<T>>>,
          return_category<3, owning_view<std::remove_reference_t<T>>>
      >;

  template<typename R, typename T>
  constexpr T operator()(R&& r, return_category<1, T>) const {
    return std::forward<R>(r);
  }

  template<typename R, typename T>
  constexpr T operator()(R&& r, return_category<2, T>) const {
    return {std::forward<R>(r)}; // ref_view
  }

  template<typename R, typename T>
  constexpr T operator()(R&& r, return_category<3, T>) const {
    return {std::forward<R>(r)}; // owning_view
  }

 public:
  template<typename R>
  constexpr typename return_category_type<R&&>::return_type
  operator()(R&& r) const {
    return (*this)(std::forward<R>(r), return_category_type<R&&>{});
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::all_adaptor_closure all{};

} // namespace niebloid
using namespace niebloid;

namespace detail {

template<typename R, bool = viewable_range<R>::value /* true */>
struct all_t_impl {
  using type = decltype(views::all(std::declval<R>()));
};

template<typename R>
struct all_t_impl<R, false> {};

} // namespace detail

template<typename R>
using all_t = typename detail::all_t_impl<R>::type;

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_ALL_H_
