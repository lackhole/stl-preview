//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_BEGIN_H_
#define PREVIEW_RANGES_BEGIN_H_

#include <cstddef>
#include <type_traits>

#include "preview/__core/decay_copy.h"
#include "preview/__core/inline_variable.h"
#include "preview/__iterator/input_or_output_iterator.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/detail/tag.h"
#include "preview/__type_traits/is_class_or_enum.h"
#include "preview/__type_traits/is_complete.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename T, template<typename, typename...> class = always_true, typename = void>
struct has_member_begin : std::false_type {};
template<typename T, template<typename, typename...> class Constraint>
struct has_member_begin<T, Constraint, void_t<decltype(preview_decay_copy(std::declval<T>().begin()))>>
    : Constraint<decltype(preview_decay_copy(std::declval<T>().begin()))> {
};

namespace begin_test_adl {

template<typename T, template<typename, typename...> class = always_true, typename = void>
struct has_adl_begin : std::false_type {};
template<typename T, template<typename, typename...> class Constraint>
struct has_adl_begin<T, Constraint, void_t<decltype(preview_decay_copy(begin(std::declval<T>())))>>
    : Constraint<decltype(preview_decay_copy(begin(std::declval<T>())))> {
};

} // namespace begin_test_adl

struct begin_niebloid {
 private:
  template<typename T>
  using tag = preview::detail::conditional_tag<
      std::is_array<remove_cvref_t<T>>,
      has_member_begin<T, input_or_output_iterator>,
      conjunction<
          is_class_or_enum<remove_cvref_t<T>>,
          begin_test_adl::has_adl_begin<T, input_or_output_iterator>
      >
  >;

  template<typename T>
  constexpr auto call(T&& t, preview::detail::tag_1) const {
    static_assert(is_complete<std::remove_all_extents_t<std::remove_reference_t<T>>>::value, "Array element must be complete type");
    return t + 0;
  }

  template<typename T>
  constexpr auto call(T&& t, preview::detail::tag_2) const {
    return preview_decay_copy(t.begin());
  }

 public:
  template<typename T, std::enable_if_t<conjunction<
      negation<conjunction<
          std::is_rvalue_reference<T&&>,
          bool_constant<( enable_borrowed_range<std::remove_cv_t<T>> == false )>
      >>,
      bool_constant<(tag<T>::value > 0)>
  >::value, int> = 0>
  constexpr auto operator()(T&& t) const {
    return call(std::forward<T>(t), tag<T>{});
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::begin_niebloid begin{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_BEGIN_H_
