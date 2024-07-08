//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_BEGIN_H_
#define PREVIEW_RANGES_BEGIN_H_

#include <cstddef>
#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/input_or_output_iterator.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__core/decay_copy.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/is_complete.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename T, typename = void>
struct has_member_begin : std::false_type {};
template<typename T>
struct has_member_begin<T, void_t<decltype( preview_decay_copy(std::declval<T>().begin()) )>>
       : input_or_output_iterator<decltype( preview_decay_copy(std::declval<T>().begin()) )> {};

template<typename T, typename = void>
struct has_adl_begin : std::false_type {};
template<typename T>
struct has_adl_begin<T, void_t<decltype( preview_decay_copy(begin(std::declval<T>())) )>>
    : input_or_output_iterator<decltype( preview_decay_copy(begin(std::declval<T>())) )> {};

struct begin_niebloid {
 private:
  template<int v>
  using tag_t = std::integral_constant<int, v>;

  template<typename T>
  constexpr auto call(T&& t, tag_t<0>) const {
    static_assert(is_complete<std::remove_all_extents_t<std::remove_reference_t<T>>>::value, "Array element must be complete type");
    return t + 0;
  }

  template<typename T>
  constexpr auto call(T&& t, tag_t<1>) const {
    return preview_decay_copy(t.begin());
  }

  template<typename T>
  constexpr auto call(T&& t, tag_t<2>) const {
    return preview_decay_copy(begin(t));
  }

 public:
  template<typename T, std::enable_if_t<conjunction<
      disjunction<
          std::is_lvalue_reference<T>,
          bool_constant<enable_borrowed_range<std::remove_cv_t<T>>>
      >,
      disjunction<
          std::is_array<remove_cvref_t<T>>,
          has_member_begin<T>,
          has_adl_begin<T>
      >
  >::value, int> = 0>
  constexpr auto operator()(T&& t) const {
    using tag = conditional_t<
        std::is_array<remove_cvref_t<T>>, tag_t<0>,
        has_member_begin<T>,              tag_t<1>,
        /* has_adl_begin<T> */            tag_t<2>
    >;
    return call(std::forward<T>(t), tag{});
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
