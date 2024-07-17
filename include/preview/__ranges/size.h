//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_SIZE_H_
#define PREVIEW_RANGES_SIZE_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__core/decay_copy.h"
#include "preview/__concepts/integer_like.h"
#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__ranges/disabled_sized_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/sentinel_t.h"
#include "preview/__type_traits/detail/tag.h"
#include "preview/__type_traits/is_bounded_array.h"
#include "preview/__type_traits/is_unbounded_array.h"
#include "preview/__type_traits/void_t.h"
#include "preview/__utility/to_unsigned_like.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename T, typename = void>
struct has_mem_fn_size : std::false_type {};
template<typename T>
struct has_mem_fn_size<T, void_t<decltype( std::declval<T>().size() )>> : integer_like<decltype(std::declval<T>().size())> {};

template<typename T, typename = void>
struct has_adl_size : std::false_type {};
template<typename T>
struct has_adl_size<T, void_t<decltype( size(std::declval<T>()) )>> : integer_like<decltype(size(std::declval<T>()))> {};

struct size_niebloid {
 private:
  template<typename T, bool = /* false */ range<T>::value, typename = void>
  struct subtractable_begin_end : std::false_type {};
  template<typename T>
  struct subtractable_begin_end<T, true, void_t<decltype(std::declval<iterator_t<T>>() - std::declval<sentinel_t<T>>())>> :
      conjunction<
          integer_like<decltype(std::declval<iterator_t<T>>() - std::declval<sentinel_t<T>>())>,
          sized_sentinel_for<sentinel_t<T>, iterator_t<T>>,
          forward_iterator<iterator_t<T>>
      >{};

  template<typename T>
  using size_callable_tag = preview::detail::conditional_tag<
      std::is_array<remove_cvref_t<T>>, // 1
      conjunction<
          negation<disabled_sized_range<std::remove_cv_t<T>>>,
          has_mem_fn_size<T>
      >, // 2
      conjunction<
          is_class_or_enum<T>,
          negation<disabled_sized_range<std::remove_cv_t<T>>>,
          has_adl_size<T>
      >, // 3
      subtractable_begin_end<T> // 4
  >;

  using tag_1 = preview::detail::tag_1;
  using tag_2 = preview::detail::tag_2;
  using tag_3 = preview::detail::tag_3;
  using tag_4 = preview::detail::tag_4;

  template<typename T>
  constexpr auto call(T&&, tag_1) const {
    return preview_decay_copy(std::extent<std::remove_reference_t<T>>::value);
  }

  template<typename T>
  constexpr auto call(T&& t, tag_2) const {
    return preview_decay_copy(t.size());
  }

  template<typename T>
  constexpr auto call(T&& t, tag_3) const {
    return preview_decay_copy(size(t));
  }

  template<typename T>
  constexpr auto call(T&& t, tag_4) const {
    return preview::to_unsigned_like(ranges::end(t) - ranges::begin(t));
  }

 public:
  template<typename T, std::enable_if_t<conjunction<
      negation<is_unbounded_array<remove_cvref_t<T>>>,
      bool_constant<(size_callable_tag<T>::value != 0)>
  >::value, int> = 0>
  constexpr auto operator()(T&& t) const {
    return call(std::forward<T>(t), size_callable_tag<T>{});
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
