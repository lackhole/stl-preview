//
// Created by yonggyulee on 2024/01/03.
//

#ifndef PREVIEW_RANGES_VIEWS_TAKE_H_
#define PREVIEW_RANGES_VIEWS_TAKE_H_

#include <algorithm>
#include <cstddef>
#include <type_traits>

#include "preview/__core/decay_copy.h"
#include "preview/__core/cxx_version.h"
#include "preview/__concepts/convertible_to.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/subrange.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/range_adaptor.h"
#include "preview/__ranges/range_adaptor_closure.h"
#include "preview/__ranges/range_difference_t.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/empty_view.h"
#include "preview/__ranges/views/iota_view.h"
#include "preview/__ranges/views/repeat.h"
#include "preview/__ranges/views/repeat_view.h"
#include "preview/__ranges/views/take_view.h"
#include "preview/span.h"
#include "preview/string_view.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/detail/return_category.h"
#include "preview/__type_traits/is_specialization.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/type_identity.h"
#include "preview/__utility/forward_like.h"

#if PREVIEW_CXX_VERSION >= 17
#include <string_view>
#endif
#if PREVIEW_CXX_VERSION >= 20
#include <span>
#endif

namespace preview {
namespace ranges {
namespace views {
namespace detail {

using preview::detail::return_category;

struct take_niebloid {
 private:
  // (1) empty_view
  template<typename R>
  constexpr auto call(R&& r, range_difference_t<R> count, return_category<1>) const {
    return ((void)count, preview_decay_copy(std::forward<R>(r)));
  }

  template<typename T>
  struct return_category_span : std::false_type {
    using category = return_category<0>;
  };
  template<typename U, std::size_t Extent>
  struct return_category_span<span<U, Extent>> : std::true_type {
    using category = return_category<2, span<U>>;
  };
#if PREVIEW_CXX_VERSION >= 20
  template<typename U, std::size_t Extent>
  struct return_category_span<std::span<U, Extent>> : std::true_type {
    using category = return_category<2, std::span<U>>;
  };
#endif

  template<typename T, bool = disjunction<
      is_specialization<T, basic_string_view>
#if PREVIEW_CXX_VERSION >= 17
      , is_specialization<T, std::basic_string_view>
#endif
  >::value /* true */>
  struct return_category_string_view : std::true_type {
    using category = return_category<2, T>;
  };
  template<typename T>
  struct return_category_string_view<T, false> : std::false_type {
    using category = return_category<0>;
  };

  template<typename T, bool = conjunction<ranges::detail::is_subrange<T>, random_access_range<T>, sized_range<T>>::value /* true */>
  struct return_category_subrange : std::true_type {
    using category = return_category<2, subrange<iterator_t<T>>>;
  };
  template<typename T>
  struct return_category_subrange<T, false> : std::false_type {
    using category = return_category<0>;
  };

  // (2) span, string_view, subrange
  template<typename R, typename U>
  constexpr U call(R&& e, range_difference_t<R> f, return_category<2, U>) const {
    using D = range_difference_t<decltype((e))>;
    auto size = (std::min<D>)(ranges::distance(e), f);
    auto first = ranges::begin(e);
    return U(first, first + size);
  }

  // (3) iota_view
  template<typename R>
  constexpr auto call(R&& e, ranges::range_difference_t<R> f, return_category<3>) const {
    using D = ranges::range_difference_t<decltype((e))>;
    auto size = (std::min<D>)(ranges::distance(e), f);
    auto first = ranges::begin(e);
    return views::iota(*first, *(first + size));
  }

  // (4) repeat_view
  template<typename R>
  constexpr auto call(R&& e, ranges::range_difference_t<R> f, return_category<4, std::true_type /* sized_range */>) const {
    using D = ranges::range_difference_t<decltype((e))>;
    return views::repeat(
        preview::force_forward_like<R>(*e.begin()), // *e.value_
        (std::min<D>)(ranges::distance(e), f)
    );
  }
  template<typename R>
  constexpr auto call(R&& e, ranges::range_difference_t<R> f, return_category<4, std::false_type /* sized_range */>) const {
    using D = ranges::range_difference_t<decltype((e))>;
    return views::repeat(
        preview::force_forward_like<R>(*e.begin()), // *e.value_
        static_cast<D>(f)
    );
  }

  // (5) others
  template<typename R>
  constexpr auto call(R&& r, ranges::range_difference_t<R> f, return_category<5>) const {
      return take_view<views::all_t<R>>(std::forward<R>(r), f);
  }

  // TODO: Check std::ranges too
  template<typename T>
  using category =
      conditional_t<
          is_specialization<T, empty_view>,     return_category<1>, // 1
          return_category_span<T>,              typename return_category_span<T>       ::category, // 2
          return_category_string_view<T>,       typename return_category_string_view<T>::category, // 2
          return_category_subrange<T>,          typename return_category_subrange<T>   ::category, // 2
          conjunction<
              is_specialization<T, iota_view>,
              random_access_range<T>,
              sized_range<T>
          >,                                    return_category<3>, // 3
          is_specialization<T, repeat_view>,    return_category<4, bool_constant<sized_range<T>::value>>, // 4
          return_category<5> // 5
      >;

 public:
  template<typename R, std::enable_if_t<ranges::viewable_range<R>::value, int> = 0>
  constexpr auto operator()(R&& r, ranges::range_difference_t<R> count) const {
    using T = remove_cvref_t<decltype((r))>;
    return call(std::forward<R>(r), std::move(count), category<T>{});
  }

  template<typename DifferenceType>
  constexpr auto operator()(DifferenceType&& count) const {
    return range_adaptor<take_niebloid, std::decay_t<DifferenceType>>(std::forward<DifferenceType>(count));
  }
};
} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::take_niebloid take{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_TAKE_H_
