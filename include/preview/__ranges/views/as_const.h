//
// Created by yonggyulee on 2024/01/22.
//

#ifndef PREVIEW_RANGES_VIEWS_AS_CONST_H_
#define PREVIEW_RANGES_VIEWS_AS_CONST_H_

#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__ranges/constant_range.h"
#include "preview/__ranges/range_adaptor_closure.h"
#include "preview/__ranges/ref_view.h"
#include "preview/__ranges/viewable_range.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/as_const_view.h"
#include "preview/__span/__forward_declare.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/detail/return_category.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

using preview::detail::return_category;

struct as_const_adaptor_closure : public range_adaptor_closure<as_const_adaptor_closure> {
 private:
  template<typename T, bool = is_invocable<decltype(views::all), T>::value /* true */>
  struct check_all : constant_range<all_t<T>> {};
  template<typename T>
  struct check_all<T, false> : std::false_type {};

  template<typename U>
  struct check_span : std::false_type {
    using category = return_category<2>;
  };
  template<typename X, std::size_t Extent>
  struct check_span<span<X, Extent>> : std::true_type {
    using category = return_category<2, span<const X, Extent>>;
  };

  template<typename T>
  struct check_ref_view : std::false_type {
    using category = return_category<3>;
  };
  template<typename X>
  struct check_ref_view<ranges::ref_view<X>> : constant_range<const X> {
    using category = return_category<3, X>;
  };

  template<typename T>
  struct check_ref_view_2
      : conjunction<
            std::is_lvalue_reference<T>,
            constant_range<const remove_cvref_t<T>>,
            negation<view<remove_cvref_t<T>>>
        > {};

  template<typename T>
  using category =
      std::conditional_t<
          check_all<T>::value, return_category<1>,
      std::conditional_t<
          check_span<remove_cvref_t<T>>::value, typename check_span<remove_cvref_t<T>>::category,
      std::conditional_t<
          check_ref_view<remove_cvref_t<T>>::value, return_category<3>,
      std::conditional_t<
          check_ref_view_2<T>::value, return_category<4>,
          return_category<0>
      >>>>;

  template<typename R>
  constexpr all_t<R&&> operator()(R&& r, return_category<1>) const {
    return views::all(std::forward<R>(r));
  }

  template<typename R, typename To>
  constexpr To operator()(R&& r, return_category<2, To>) const {
    return To(std::forward<R>(r));
  }

  template<typename R, typename X>
  constexpr ref_view<const X>
  operator()(R&& r, return_category<3, X>) const {
    return ref_view<const X>(static_cast<const X&>(std::forward<R>(r).base()));
  }

  template<typename R>
  constexpr ref_view<const remove_cvref_t<R>>
  operator()(R&& r, return_category<4>) const {
    using U = remove_cvref_t<R>;
    return ref_view<const U>(static_cast<const U&>(r));
  }

  template<typename R>
  constexpr as_const_view<all_t<R>>
  operator()(R&& r, return_category<0>) const {
    return as_const_view<all_t<R>>(std::forward<R>(r));
  }

 public:
  template<typename R, std::enable_if_t<viewable_range<R>::value, int> = 0>
  constexpr auto operator()(R&& r) const {
    return (*this)(std::forward<R>(r), category<R&&>{});
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::as_const_adaptor_closure as_const{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_AS_CONST_H_
