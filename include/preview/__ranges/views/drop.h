//
// Created by yonggyulee on 2024/01/29.
//

#ifndef PREVIEW_RANGES_VIEWS_DROP_H_
#define PREVIEW_RANGES_VIEWS_DROP_H_

#include <algorithm>
#include <cstddef>
#include <type_traits>

#include "preview/core.h"

#if PREVIEW_CXX_VERSION >= 17
#include <string_view>
#endif
#if PREVIEW_CXX_VERSION >= 20
#include <span>
#endif

#include "preview/__concepts/different_from.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/range_adaptor_closure.h"
#include "preview/__ranges/subrange.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/drop_view.h"
#include "preview/__ranges/views/empty_view.h"
#include "preview/__ranges/views/iota_view.h"
#include "preview/__ranges/views/repeat.h"
#include "preview/__ranges/views/repeat_view.h"
#include "preview/span.h"
#include "preview/string_view.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/detail/return_category.h"
#include "preview/__type_traits/is_specialization.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__utility/forward_like.h"
#include "preview/__utility/to_unsigned_like.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

using preview::detail::return_category;

struct drop_niebloid {
 private:
  // subrange
  template<typename T, bool = conjunction<ranges::detail::is_subrange<T>, random_access_range<T>, sized_range<T>>::value /* true */>
  struct return_category_subrange : std::true_type {
    using category = std::conditional_t<ranges::detail::is_size_storing_subrange<T>::value,
        return_category<3, T>,
        return_category<2, T>
    >;
  };
  template<typename T>
  struct return_category_subrange<T, false> : std::false_type {
    using category = return_category<0>;
  };

  // span
  template<typename T>
  struct return_category_span : std::false_type {
    using category = return_category<0>;
  };
  template<typename T, std::size_t Extent>
  struct return_category_span<span<T, Extent>> : std::true_type {
    using category = return_category<2, span<T>>;
  };
#if PREVIEW_CXX_VERSION >= 20
  template<typename T, std::size_t Extent>
  struct return_category_span<std::span<T, Extent>> : std::true_type {
    using category = return_category<2, std::span<T>>;
  };
#endif

  template<typename T>
  using is_basic_string_view = disjunction<
      is_specialization<T, basic_string_view>
#if PREVIEW_CXX_VERSION >= 17
      , is_specialization<T, std::basic_string_view>
#endif
  >;

  // 2.1 empty_view
  template<typename R, typename D>
  constexpr auto call(R&& r, D, return_category<1>) const {
    return preview_decay_copy(std::forward<R>(r));
  }

  // 2.2 span, basic_string_view, iota_view, subrange (StoreSize == false)
  template<typename R, typename D, typename U>
  constexpr U call(R&& e, D f, return_category<2, U>) const {
    return U(
        ranges::begin(e) + (std::min<D>)(ranges::distance(e), f),
        ranges::end(e)
    );
  }

  // 2.3 subrange
  template<typename R, typename D, typename T>
  constexpr T call(R&& e, D f, return_category<3, T>) const {
    auto inc = (std::min<D>)(ranges::distance(e), f);
    return T(
        ranges::begin(e) + inc,
        ranges::end(e),
        preview::to_unsigned_like(ranges::distance(e) - inc)
    );
  }

  // 2.4. repeat_view
  template<typename R, typename D>
  constexpr auto call(R&& e, D f, return_category<4, std::true_type /* sized_range */>) const {
    return views::repeat(
        preview::force_forward_like<R>(*e.begin()), // *e.value_
        ranges::distance(e) - (std::min<D>)(ranges::distance(e), f)
    );
  }
  template<typename R, typename D>
  constexpr auto call(R&& e, D f, return_category<4, std::false_type /* sized_range */>) const {
    return ((void)f, preview_decay_copy(e));
  }

  // 2.5 drop_view
  template<typename R, typename D>
  constexpr drop_view<all_t<R>> call(R&& r, D f, return_category<5>) const {
      return drop_view<all_t<R>>(std::forward<R>(r), f);
  }

  template<typename T>
  using category = conditional_t<
      is_specialization<T, empty_view>,  return_category<1>,
      return_category_span<T>,           typename return_category_span<T>::category,        // 2
      is_basic_string_view<T>,           return_category<2, T>,
      conjunction<
          is_specialization<T, iota_view>,
          sized_range<T>,
          random_access_range<T>
      >,                                 return_category<2, T>,
      return_category_subrange<T>,       typename return_category_subrange<T>::category,    // 2 or 3
      is_specialization<T, repeat_view>, return_category<4, bool_constant<sized_range<T>::value>>,
      return_category<5>
  >;

 public:
  template<typename R, std::enable_if_t<viewable_range<R>::value, int> = 0>
  constexpr auto operator()(R&& r, range_difference_t<R> count) const {
    using T = remove_cvref_t<decltype((r))>;
    return call(std::forward<R>(r), std::move(count), category<T>{});
  }

  template<typename DifferenceType>
  constexpr auto operator()(DifferenceType&& count) const {
    return range_adaptor<drop_niebloid, std::decay_t<DifferenceType>>(std::forward<DifferenceType>(count));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::drop_niebloid drop{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_DROP_H_
