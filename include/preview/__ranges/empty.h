//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_EMPTY_H_
#define PREVIEW_RANGES_EMPTY_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/size.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/detail/tag.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/void_t.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

struct empty_niebloid {
 private:
  template<typename T, typename = void>
  struct has_mem_fn_empty : std::false_type {};
  template<typename T>
  struct has_mem_fn_empty<T, void_t<decltype( bool(std::declval<T>().empty()) )>> : std::true_type {};

  template<typename R, bool = forward_range<R>::value /* false */>
  struct begin_end_comparable_range : std::false_type {};
  template<typename R>
  struct begin_end_comparable_range<R, true>
      : rel_ops::is_equality_comparable<iterator_t<R>, sentinel_t<R>> {};

  using tag_1 = preview::detail::tag_1;
  using tag_2 = preview::detail::tag_2;
  using tag_3 = preview::detail::tag_3;

  template<typename T>
  constexpr bool call(T&& t, tag_1) const {
    return bool(t.empty());
  }

  template<typename T>
  constexpr bool call(T&& t, tag_2) const {
    return (ranges::size(t) == 0);
  }

  template<typename T>
  constexpr bool call(T&& t, tag_3) const {
    using namespace preview::rel_ops;
    return bool(ranges::begin(t) == ranges::end(t));
  }

  template<typename T>
  using callable_tag = preview::detail::conditional_tag<
      has_mem_fn_empty<T>,
      is_invocable<decltype(ranges::size), T>,
      begin_end_comparable_range<T>
  >;

 public:
  template<typename T, std::enable_if_t<(callable_tag<T>::value != 0), int> = 0>
  constexpr bool operator()(T&& t) const {
    return call(std::forward<T>(t), callable_tag<T>{});
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::empty_niebloid empty{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_EMPTY_H_
