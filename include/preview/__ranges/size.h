//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_SIZE_H_
#define PREVIEW_RANGES_SIZE_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__core/decay_copy.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__concepts/subtractable.h"
#include "preview/__ranges/disable_sized_range.h"
#include "preview/__ranges/range.h"
#include "preview/__type_traits/is_bounded_array.h"
#include "preview/__type_traits/is_class_or_enum.h"
#include "preview/__type_traits/is_integer_like.h"
#include "preview/__type_traits/void_t.h"
#include "preview/__utility/to_unsigned_like.h"

namespace preview {
namespace ranges {
namespace detail {

struct size_niebloid {
 private:
  template<typename T, typename = void>
  struct have_member_size : std::false_type {};
  template<typename T>
  struct have_member_size<T, void_t<decltype( std::declval<T>().size() )>>
    : is_integer_like<decltype(std::declval<T>().size())> {};

  template<typename T, typename = void>
  struct have_adl_size : std::false_type {};
  template<typename T>
  struct have_adl_size<T, void_t<decltype( size(std::declval<T>()) )>>
    : is_integer_like<decltype(size(std::declval<T>()))> {};

  template<typename T, typename I, typename S, bool = preview::detail::is_explicitly_subtractable<S, I>::value /* false */>
  struct subtractable_begin_end_impl : std::false_type {};
  template<typename T, typename I, typename S>
  struct subtractable_begin_end_impl<T, I, S, true>
      : conjunction<
          is_integer_like<decltype(std::declval<S>() - std::declval<I>())>,
          sized_sentinel_for<S, I>,
          forward_iterator<I>
      >{};

  template<typename T, bool = range<T>::value /* false */>
  struct subtractable_begin_end : std::false_type {};
  template<typename T>
  struct subtractable_begin_end<T, true>
      : subtractable_begin_end_impl<
          T,
          decltype(ranges::begin(std::declval<T>())),
          decltype(ranges::end  (std::declval<T>()))
      > {};

  template<typename T>
  constexpr auto call(T&&, std::integral_constant<int, 0>) const {
    return preview_decay_copy(std::extent<T>::value);
  }
  template<typename T>
  constexpr auto call(T&& t, std::integral_constant<int, 1>) const {
    return preview_decay_copy(t.size());
  }
  template<typename T>
  constexpr auto call(T&& t, std::integral_constant<int, 2>) const {
    return preview_decay_copy(size(t));
  }
  template<typename T>
  constexpr auto call(T&& t, std::integral_constant<int, 3>) const {
    return preview::to_unsigned_like(ranges::end(t) - ranges::begin(t));
  }

 public:
  template<typename T, std::enable_if_t<conjunction<
      negation<is_unbounded_array<T>>,
      disjunction<
          is_bounded_array<T>,
          conjunction< negation<disable_sized_range<T>>, have_member_size<T> >,
          conjunction< negation<disable_sized_range<T>>, have_adl_size<T>    >,
          subtractable_begin_end<T>
      >
  >::value, int> = 0>
  constexpr auto operator()(T&& t) const {
    using tag = conditional_t<
        is_bounded_array<T>,                                                  std::integral_constant<int, 0>,
        conjunction< negation<disable_sized_range<T>>, have_member_size<T> >, std::integral_constant<int, 1>,
        conjunction< negation<disable_sized_range<T>>, have_adl_size<T>    >, std::integral_constant<int, 2>,
        /* subtractable_begin_end */                                          std::integral_constant<int, 3>
    >;
    return call(std::forward<T>(t), tag{});
  }
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::size_niebloid size{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_SIZE_H_
