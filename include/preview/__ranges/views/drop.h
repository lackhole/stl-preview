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
#include "preview/__utility/to_unsigned_like.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

using preview::detail::return_category;

struct drop_niebloid {
 private:
  // empty_view - 1
  template<typename R, typename T, typename D, bool = is_specialization<T, empty_view>::value /* true */>
  struct return_category_empty_view : std::true_type {
    using category = return_category<1, decltype(preview_decay_copy(std::declval<R>()))>;
  };
  template<typename R, typename T, typename D>
  struct return_category_empty_view<R, T, D, false> : std::false_type {
    using category = return_category<0>;
  };
  template<typename R, typename D, typename RT>
  constexpr RT operator()(R&& r, D, return_category<1, RT>) const {
    return std::forward<R>(r);
  }

  // subrange - 2 / 3
  template<typename T, bool = conjunction<ranges::detail::is_subrange<T>, random_access_range<T>, sized_range<T>>::value /* true */>
  struct return_category_subrange : std::true_type {
    using category = std::conditional_t<ranges::detail::is_size_storing_subrange<T>::value,
        return_category<2, T>,
        return_category<3, T>
    >;
  };
  template<typename T>
  struct return_category_subrange<T, false> : std::false_type {
    using category = return_category<0>;
  };

  template<typename R, typename D, typename T>
  constexpr T operator()(R&& e, D f, return_category<2, T>) const {
    auto inc = (std::min<D>)(ranges::distance(e), f);
    return T(
        ranges::begin(e) + inc,
        ranges::end(e),
        preview::to_unsigned_like(ranges::distance(e) - inc)
    );
  }

  // span - 3
  template<typename T>
  struct is_span : std::false_type {};
  template<typename T, std::size_t Extent>
  struct is_span<span<T, Extent>> : std::true_type {};

  template<typename T, typename D, bool = is_span<T>::value /* true */>
  struct return_category_span : std::true_type {
    using category = return_category<3, span<typename T::element_type>>;
  };
  template<typename T, typename D>
  struct return_category_span<T, D, false> : std::false_type {
    using category = return_category<0>;
  };

  // basic_string_view - 3
  template<typename T, bool = disjunction<
      is_specialization<T, basic_string_view>
#if PREVIEW_CXX_VERSION >= 17
      , is_specialization<T, std::basic_string_view>
#endif
  >::value /* true */>
  struct return_category_string_view : std::true_type {
    using category = return_category<3, T>;
  };
  template<typename T>
  struct return_category_string_view<T, false> : std::false_type {
    using category = return_category<0>;
  };

  // iota_view - 3
  template<typename T, bool = conjunction<
      is_specialization<T, iota_view>,
      sized_range<T>,
      random_access_range<T>
  >::value /* true */>
  struct return_category_iota_view : std::true_type {
    using category = return_category<3, T>;
  };
  template<typename T>
  struct return_category_iota_view<T, false> : std::false_type {
    using category = return_category<0>;
  };

  // TODO: Investigate for subrange fallback
  template<typename R, typename D, typename U>
  constexpr U operator()(R&& e, D f, return_category<3, U>) const {
    return U(
        ranges::begin(e) + (std::min<D>)(ranges::distance(e), f),
        ranges::end(e)
    );
  }

  // repeat_view - 4
  template<typename T, bool = is_specialization<T, repeat_view>::value /* false */>
  struct return_category_repeat_view : std::false_type {
    using category = return_category<0>;
  };
  template<typename T>
  struct return_category_repeat_view<T, true> : std::true_type {
    using category = return_category<4, bool_constant<sized_range<T>::value>>;
  };
  template<typename R, typename D>
  constexpr auto operator()(R&& e, D f, return_category<4, std::true_type /* sized_range */>) const {
    return views::repeat(*e, ranges::distance(e) - (std::min<D>)(ranges::distance(e), f));
  }
  template<typename R, typename D>
  constexpr auto operator()(R&& e, D, return_category<4, std::false_type /* sized_range */>) const {
    return preview_decay_copy(e);
  }

  // drop_view - 0 (default)
  template<typename R, typename D>
  constexpr drop_view<all_t<R>> operator()(R&& r, D f, return_category<0>) const {
      return drop_view<all_t<R>>(std::forward<R>(r), f);
  }

  template<typename R, typename T, typename D>
  using category =
      conditional_t<
          return_category_empty_view<R, T, D>, typename return_category_empty_view<R, T, D>::category, // 1
          return_category_span<T, D>, typename return_category_span<T, D>::category, // 3
          return_category_string_view<T>, typename return_category_string_view<T>::category, // 3
          return_category_subrange<T>, typename return_category_subrange<T>::category, // 2 or 3
          return_category_iota_view<T>, typename return_category_iota_view<T>::category, // 3
          return_category_repeat_view<T>, typename return_category_iota_view<T>::category, // 4
          return_category<0>
      >;

 public:
  template<typename R, std::enable_if_t<viewable_range<R>::value, int> = 0>
  constexpr auto
  operator()(R&& r, range_difference_t<R> count) const {
    using T = remove_cvref_t<decltype((r))>;
    using D = range_difference_t<decltype((r))>;
    return (*this)(std::forward<R>(r), count, category<R&&, T, D>{});
  }

  template<typename DifferenceType>
  constexpr auto operator()(DifferenceType&& count) const {
    return range_adaptor<drop_niebloid, std::remove_reference_t<DifferenceType>>(std::forward<DifferenceType>(count));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::drop_niebloid drop{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_DROP_H_
