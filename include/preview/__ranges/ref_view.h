//
// Created by yonggyulee on 2024/01/02.
//

#ifndef PREVIEW_RANGES_REF_VIEW_H_
#define PREVIEW_RANGES_REF_VIEW_H_

#include <memory>
#include <type_traits>

#include "preview/__concepts/convertible_to.h"
#include "preview/__concepts/different_from.h"
#include "preview/__memory/addressof.h"
#include "preview/__ranges/contiguous_range.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/data.h"
#include "preview/__ranges/empty.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/size.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"

namespace preview {
namespace ranges {
namespace detail {
namespace ref_view_fn {

template<typename R> static void fun(R&);
template<typename R> static void fun(R&&) = delete;

template<typename R, typename T, typename = void>
struct fallback_to_lref : std::false_type {};
template<typename R, typename T>
struct fallback_to_lref<R, T, void_t<decltype(fun<R>(std::declval<T>()))>> : std::true_type {};

} // namespace ref_view_fn
} // namespace detail

template<typename R>
class ref_view : public view_interface<ref_view<R>> {
 public:
  static_assert(range<R>::value, "Constraints not satisfied");
  static_assert(std::is_object<R>::value, "Constraints not satisfied");

  template<typename T, std::enable_if_t<conjunction<
      different_from<T, ref_view>,
      convertible_to<T, R&>,
      detail::ref_view_fn::fallback_to_lref<R, T>
  >::value, int> = 0>
  PREVIEW_CONSTEXPR_AFTER_CXX17 ref_view(T&& t) noexcept
      : r_(preview::addressof(static_cast<R&>(std::forward<T>(t)))) {}

  constexpr R& base() const {
    return *r_;
  }

  constexpr iterator_t<R> begin() const {
    return ranges::begin(*r_);
  }

  constexpr sentinel_t<R> end() const {
    return ranges::end(*r_);
  }

  template<typename T = R, std::enable_if_t<
      is_invocable<decltype(ranges::empty), T&>::value, int> = 0>
  constexpr bool empty() const {
    return ranges::empty(*r_);
  }

  template<typename T = R, std::enable_if_t<
      sized_range<T>::value, int> = 0>
  constexpr auto size() const {
    return ranges::size(*r_);
  }

  template<typename T = R, std::enable_if_t<
      contiguous_range<T>::value, int> = 0>
  constexpr auto data() const {
    return ranges::data(*r_);
  }

 private:
  R* r_;
};

#if __cplusplus >= 201703L

template<typename R>
ref_view(R&) -> ref_view<R>;

#endif

} // namespace ranges
} // namespace namespace preview

template<typename T>
PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(preview::ranges::ref_view<T>) = true;

#endif // PREVIEW_RANGES_REF_VIEW_H_
