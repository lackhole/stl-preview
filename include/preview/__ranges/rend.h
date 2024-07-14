//
// Created by yonggyulee on 2024/02/08.
//

#ifndef PREVIEW_RANGES_REND_H_
#define PREVIEW_RANGES_REND_H_

#include <iterator>
#include <type_traits>

#include "preview/__core/decay_copy.h"
#include "preview/__core/inline_variable.h"
#include "preview/__iterator/bidirectional_iterator.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/common_range.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__ranges/detail/not_incomplete_array.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/rbegin.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {


struct rend_niebloid {
 private:
  using tag_1 = preview::detail::tag_1;
  using tag_2 = preview::detail::tag_2;
  using tag_3 = preview::detail::tag_3;

  template<typename T, bool = is_invocable<rbegin_niebloid, T&&>::value, typename = void>
  struct rend_member_check : std::false_type {};
  template<typename T>
  struct rend_member_check<T,
        true, void_t<decltype(preview_decay_copy( std::declval<T>().rend() ))>>
      : sentinel_for<decltype(preview_decay_copy( std::declval<T>().rend() )),
                     decltype( ranges::rbegin(std::declval<T>()) )> {};

  template<typename T, bool = conjunction<
      is_invocable<rbegin_niebloid, T&&>,
      is_class_or_enum<remove_cvref_t<T>>>::value, typename = void>
  struct rend_global_check : std::false_type {};
  template<typename T>
  struct rend_global_check<T,
        true, void_t<decltype(preview_decay_copy( rend(std::declval<T>()) ))>>
      : sentinel_for<decltype(preview_decay_copy( rend(std::declval<T>()) )),
                     decltype( ranges::rbegin(std::declval<T>()) )> {};

  template<typename T, bool = common_range<T>::value>
  struct common_bidi_check : std::false_type {};
  template<typename T>
  struct common_bidi_check<T, true> : bidirectional_iterator<decltype( ranges::begin(std::declval<T>()) )> {};

  template<typename T>
  using rend_tag = preview::detail::conditional_tag<rend_member_check<T>, rend_global_check<T>, common_bidi_check<T>>;

  template<typename T>
  constexpr auto run(T&& t, tag_1) const {
    return preview_decay_copy(t.rend());
  }

  template<typename T>
  constexpr auto run(T&& t, tag_2) const {
    return preview_decay_copy(rend(t));
  }

  template<typename T>
  constexpr auto run(T&& t, tag_3) const {
    return std::make_reverse_iterator(ranges::begin(t));
  }

 public:
  template<typename T, std::enable_if_t<conjunction<
      preview::detail::not_incomplete_array<T>,
      disjunction<
        std::is_lvalue_reference<std::remove_cv_t<T>>,
        enable_borrowed_range_t<std::remove_cv_t<T>>
      >,
      bool_constant<(rend_tag<T>::value > 0)>
  >::value, int> = 0>
  constexpr auto operator()(T&& t) const {
    return run(std::forward<T>(t), rend_tag<T>{});
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::rend_niebloid rend{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_REND_H_
