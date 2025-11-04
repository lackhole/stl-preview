//
// Created by yonggyulee on 28/04/2025
//

#ifndef PREVIEW_RANGES_RESERVE_HINT_H_
#define PREVIEW_RANGES_RESERVE_HINT_H_

#include <type_traits>

#include "preview/__core/decay_copy.h"
#include "preview/__core/inline_variable.h"
#include "preview/__concepts/integer_like.h"
#include "preview/__concepts/invocable.h"
#include "preview/__ranges/size.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/detail/tag.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {
namespace reserve_hint_test_adl {

template<typename T, typename = void>
struct has_adl_reserve_hint : std::false_type {};
template<typename T>
struct has_adl_reserve_hint<T, void_t<decltype(reserve_hint(std::declval<T>()))>>
    : integer_like<decltype(preview_decay_copy(reserve_hint(std::declval<T>())))> {};

} // namespace reserve_hint_test_adl

struct reserve_hint_niebloid {
 private:
  template<typename T, typename = void>
  struct has_mem_fn_reserve_hint : std::false_type {};
  template<typename T>
  struct has_mem_fn_reserve_hint<T, void_t<decltype(std::declval<T>().reserve_hint())>>
      : integer_like<decltype(preview_decay_copy(std::declval<T>().reserve_hint()))> {};

  template<typename T>
  using tag = preview::detail::conditional_tag<
    invocable<decltype(ranges::size), T>,
    has_mem_fn_reserve_hint<T>,
    conjunction<
        disjunction<
            std::is_class<T>,
            std::is_enum<T>
        >,
        reserve_hint_test_adl::has_adl_reserve_hint<T>
    >
  >;

  template<typename T>
  constexpr auto call(T&& t, preview::detail::tag_1) const {
    return ranges::size(t);
  }
  template<typename T>
  constexpr auto call(T&& t, preview::detail::tag_2) const {
    return preview_decay_copy(t.reserve_hint());
  }
  template<typename T>
  constexpr auto call(T&& t, preview::detail::tag_3) const {
    return preview_decay_copy(reserve_hint(t));
  }

 public:
  template<typename T, std::enable_if_t<(tag<T>::value > 0), int> = 0>
  constexpr auto operator()(T&& t) const {
    return call(std::forward<T>(t), tag<T>{});
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::reserve_hint_niebloid reserve_hint{};

} // namespace ranges
} // namespace preview 

#endif // PREVIEW_RANGES_RESERVE_HINT_H_
