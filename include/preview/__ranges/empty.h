//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_EMPTY_H_
#define PREVIEW_RANGES_EMPTY_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/size.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/void_t.h"
#include "preview/__utility/cxx20_rel_ops.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename I, typename S>
struct begin_end_comparable_range_impl
  : conjunction<
    rel_ops::is_equality_comparable<I, S>,
    forward_iterator<I>
  > {};

struct empty_niebloid {
 private:
  template<typename T, typename = void>
  struct has_member_empty : std::false_type {};
  template<typename T>
  struct has_member_empty<T, void_t<decltype( bool(std::declval<T>().empty()) )>> : std::true_type {};

  template<typename T, typename = void>
  struct has_ranges_size : std::false_type {};
  template<typename T>
  struct has_ranges_size<T, void_t<decltype( ranges::size(std::declval<T>()) == 0 )>> : std::true_type {};

  template<typename R, bool = range<R>::value /* false */>
  struct begin_end_comparable_range : std::false_type {};
  template<typename R>
  struct begin_end_comparable_range<R, true>
    : begin_end_comparable_range_impl<
      /* I = */decltype( ranges::begin(std::declval<R>()) ),
      /* S = */decltype( ranges::end  (std::declval<R>()) )
    > {};

  template<typename T>
  constexpr bool call(T&& t, std::integral_constant<int, 0>) const {
    return bool(t.empty());
  }

  template<typename T>
  constexpr bool call(T&& t, std::integral_constant<int, 1>) const {
    return (ranges::size(t) == 0);
  }

  template<typename T>
  constexpr bool call(T&& t, std::integral_constant<int, 2>) const {
    using namespace preview::rel_ops;
    return bool(ranges::begin(t) == ranges::end(t));
  }

 public:
  template<typename T, std::enable_if_t<disjunction<
      has_member_empty<T>,
      has_ranges_size<T>,
      begin_end_comparable_range<T>
  >::value, int> = 0>
  constexpr bool operator()(T&& t) const {
    using tag = conditional_t<
        has_member_empty<T>,          std::integral_constant<int, 0>,
        has_ranges_size<T>,           std::integral_constant<int, 1>,
        /* begin_end_comparable<T> */ std::integral_constant<int, 2>
    >;
    return call(std::forward<T>(t), tag{});
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
