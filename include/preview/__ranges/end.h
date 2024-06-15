//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_END_H_
#define PREVIEW_RANGES_END_H_

#include <cstddef>
#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__type_traits/detail/return_category.h"
#include "preview/__core/decay_copy.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/is_bounded_array.h"
#include "preview/__type_traits/is_complete.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

using preview::detail::return_category;

struct end_niebloid {
 private:
  template<typename T, bool = is_bounded_array<remove_cvref_t<T>>::value>
  struct end_array_check : std::false_type {
    using category = return_category<0>;
  };
  template<typename T>
  struct end_array_check<T, true> : std::true_type {
    using category = return_category<1, decltype(std::declval<T>() + std::extent<std::remove_reference_t<T>>::value)>;
  };

  template<typename T, typename = void>
  struct end_member_check : std::false_type {
    using category = return_category<0>;
  };
  template<typename T>
  struct end_member_check<T, void_t<decltype(preview_decay_copy(std::declval<T>().end()))>>
      : sentinel_for<decltype(preview_decay_copy(std::declval<T>().end())), iterator_t<T>> {
    using category = return_category<2, decltype(preview_decay_copy(std::declval<T>().end()))>;
  };

  template<typename T, typename = void>
  struct end_global_check : std::false_type {
    using category = return_category<0>;
  };
  template<typename T>
  struct end_global_check<T, void_t<decltype(preview_decay_copy(end(std::declval<T>())))>>
      : sentinel_for<decltype(preview_decay_copy(end(std::declval<T>()))), iterator_t<T>> {
    using category = return_category<3, decltype(preview_decay_copy(end(std::declval<T>())))>;
  };

  template<typename T, bool = end_member_check<T>::value /* false */>
  struct end_category_impl_2 : end_global_check<T> {};
  template<typename T>
  struct end_category_impl_2<T, true> : end_member_check<T> {};

  template<typename T, bool = end_array_check<T>::value /* false */>
  struct end_category_impl : end_category_impl_2<T> {};
  template<typename T>
  struct end_category_impl<T, true> : end_array_check<T> {};

  template<typename T>
  struct end_category
      : std::conditional_t<
          disjunction<
            std::is_lvalue_reference<T>,
            enable_borrowed_range<remove_cvref_t<T>>
          >::value,
          typename end_category_impl<T>::category,
          return_category<0>
        > {};

  template<typename T, typename R>
  constexpr R operator()(T&& t, return_category<1, R>) const {
    static_assert(is_complete<std::remove_all_extents_t<std::remove_reference_t<T>>>::value, "Array element must be complete type");
    return t + std::extent<remove_cvref_t<T>>::value;
  }

  template<typename T, typename R>
  constexpr R operator()(T&& t, return_category<2, R>) const {
    return preview_decay_copy(t.end());
  }

  template<typename T, typename R>
  constexpr R operator()(T&& t, return_category<3, R>) const {
    return preview_decay_copy(end(t));
  }

 public:
  template<typename T>
  constexpr typename end_category<T&&>::return_type
  operator()(T&& t) const {
    return (*this)(std::forward<T>(t), end_category<T&&>{});
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::end_niebloid end{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_END_H_
