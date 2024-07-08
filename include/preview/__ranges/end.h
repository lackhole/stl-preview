//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_END_H_
#define PREVIEW_RANGES_END_H_

#include <cstddef>
#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__core/decay_copy.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/is_bounded_array.h"
#include "preview/__type_traits/is_complete.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

#if defined(__GNUC__) && (__GNUC__ < 11) && !defined(__clang__)
template<typename T, typename S, typename = void>
struct range_sentinel_for : std::false_type {};
template<typename T, typename S>
struct range_sentinel_for<T, S, void_t<decltype( ranges::begin(std::declval<T&>()) )>>
  : sentinel_for<S, decltype( ranges::begin(std::declval<T&>()) )> {};
#else
template<typename T, typename S, bool /* false */ = has_typename_type<iterator<T>>::value>
struct range_sentinel_for : std::false_type {};
template<typename T, typename S>
struct range_sentinel_for<T, S, true>
    : sentinel_for<S, iterator_t<T>> {};
#endif

template<typename T, typename = void>
struct has_member_end : std::false_type {};
template<typename T>
struct has_member_end<T, void_t<decltype( preview_decay_copy(std::declval<T>().end()) )>>
        : range_sentinel_for<T, decltype( preview_decay_copy(std::declval<T>().end()) )> {};

template<typename T, typename = void>
struct has_adl_end : std::false_type {};
template<typename T>
struct has_adl_end<T, void_t<decltype( preview_decay_copy(end(std::declval<T>())) )>>
     : range_sentinel_for<T, decltype( preview_decay_copy(end(std::declval<T>())) )> {};

struct end_niebloid {
 private:
  template<int v>
  using tag_t = std::integral_constant<int, v>;

  template<typename T>
  constexpr auto call(T&& t, tag_t<0>) const noexcept {
    static_assert(is_complete<std::remove_all_extents_t<remove_cvref_t<T>>>::value, "Array element must be complete type");
    return t + std::extent<remove_cvref_t<T>>::value;
  }

  template<typename T>
  constexpr auto call(T&& t, tag_t<1>) const {
    return preview_decay_copy(t.end());
  }

  template<typename T>
  constexpr auto call(T&& t, tag_t<2>) const {
    return preview_decay_copy(end(t));
  }

  template<typename T>
  using tag = conditional_t<
      is_bounded_array<remove_cvref_t<T>>, tag_t<0>,
      has_member_end<T>,                   tag_t<1>,
      has_adl_end<T>,                      tag_t<2>,
      tag_t<-1>
  >;
 public:
  template<typename T>
  constexpr auto operator()(T&& t) const -> decltype(call(std::forward<T>(t), tag<T>{})) {
    return call(std::forward<T>(t), tag<T>{});
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
