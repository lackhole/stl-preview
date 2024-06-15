//
// Created by yonggyulee on 2024/01/03.
//

#ifndef PREVIEW_RANGES_VIEWS_TAKE_H_
#define PREVIEW_RANGES_VIEWS_TAKE_H_

#include <algorithm>
#include <cstddef>
#include <type_traits>

#include "preview/__core/decay_copy.h"
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
#include "preview/__type_traits/detail/return_category.h"
#include "preview/__type_traits/is_specialization.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/type_identity.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

using preview::detail::return_category;

struct take_niebloid {
 private:
  template<typename R, typename T, typename D, bool = is_specialization<T, empty_view>::value /* true */>
  struct return_category_empty_view : std::true_type {
    using category = return_category<1, decltype(preview_decay_copy(std::declval<R>()))>;
  };
  template<typename R, typename T, typename D>
  struct return_category_empty_view<R, T, D, false> : std::false_type {
    using category = return_category<0>;
  };
  template<typename R, typename RT>
  constexpr RT operator()(R&& r, range_difference_t<R> count, return_category<1, RT>) const {
    return ((void)count, preview_decay_copy(std::forward<R>(r)));
  }

  template<typename T>
  struct is_span : std::false_type {};
  template<typename T, std::size_t Extent>
  struct is_span<span<T, Extent>> : std::true_type {};

  template<typename T, typename D, bool = is_span<T>::value /* true */>
  struct return_category_span : std::true_type {
    using category = return_category<2, span<typename T::element_type>>;
  };
  template<typename T, typename D>
  struct return_category_span<T, D, false> : std::false_type {
    using category = return_category<0>;
  };

  template<typename T, bool = is_specialization<T, basic_string_view>::value /* true */>
  struct return_category_string_view : std::true_type {
    using category = return_category<2, T>;
  };
  template<typename T>
  struct return_category_string_view<T, false> : std::false_type {
    using category = return_category<0>;
  };

  template<typename T, bool = ranges::detail::is_subrange<T>::value /* true */>
  struct return_category_subrange : std::true_type {
    using category = return_category<2, subrange<iterator_t<T>>>;
  };
  template<typename T>
  struct return_category_subrange<T, false> : std::false_type {
    using category = return_category<0>;
  };

  template<typename R, typename U>
  constexpr U operator()(R&& e, range_difference_t<R> f, return_category<2, U>) const {
    using D = range_difference_t<decltype((e))>;
    return U(
        ranges::begin(e),
        ranges::begin(e) + (std::min<D>)(ranges::distance(e), f)
    );
  }

  template<typename T, bool = conjunction<is_specialization<T, iota_view>, random_access_range<T>, sized_range<T>>::value /* true */>
  struct return_category_iota_view : std::true_type {
    using category = return_category<3, T>;
  };
  template<typename T>
  struct return_category_iota_view<T, false> : std::false_type {
    using category = return_category<0>;
  };
  template<typename R, typename IV>
  constexpr IV operator()(R&& e, ranges::range_difference_t<R> f, return_category<3, IV>) const {
    using D = ranges::range_difference_t<decltype((e))>;
    return views::iota(
        *ranges::begin(e),
        *(ranges::begin(e) + (std::min<D>)(ranges::distance(e), f))
    );
  }

  template<typename T, bool = is_specialization<T, repeat_view>::value /* true */ >
  struct return_category_repeat_view : std::true_type {
    using category = return_category<4, bool_constant<sized_range<T>::value>>;
  };
  template<typename T>
  struct return_category_repeat_view<T, false> : std::false_type {
    using category = return_category<0>;
  };
  template<typename R>
  constexpr auto operator()(R&& e, ranges::range_difference_t<R> f, return_category<4, std::true_type /* sized_range */>) const {
    using D = ranges::range_difference_t<decltype((e))>;
    return views::repeat(*(e.begin()), (std::min<D>)(ranges::distance(e), f));
  }
  template<typename R>
  constexpr auto operator()(R&& e, ranges::range_difference_t<R> f, return_category<4, std::false_type /* sized_range */>) const {
    using D = ranges::range_difference_t<decltype((e))>;
    return views::repeat(*(e.begin()), static_cast<D>(f));
  }

  template<typename R, typename TakeView>
  constexpr TakeView operator()(R&& r, ranges::range_difference_t<R> f, return_category<5, TakeView>) const {
      return TakeView(std::forward<R>(r), f);
  }

  template<typename R, typename T, typename D>
  using category =
      std::conditional_t<
          return_category_empty_view<R, T, D>::value, typename return_category_empty_view<R, T, D>::category, // 1
      std::conditional_t<
          return_category_span<T, D>::value, typename return_category_span<T, D>::category, // 2
      std::conditional_t<
          return_category_string_view<T>::value, typename return_category_string_view<T>::category, // 2
      std::conditional_t<
          return_category_subrange<T>::value, typename return_category_subrange<T>::category, // 2
      std::conditional_t<
          return_category_iota_view<T>::value, typename return_category_iota_view<T>::category, // 3
      std::conditional_t<
          return_category_repeat_view<T>::value, typename return_category_repeat_view<T>::category, // 4
          return_category<5, take_view<views::all_t<R>>>> // 5
      >>>>>;


 public:
  template<typename R, std::enable_if_t<ranges::viewable_range<R>::value, int> = 0>
  constexpr auto
  operator()(R&& r, ranges::range_difference_t<R> count) const {
    using T = remove_cvref_t<decltype((r))>;
    using D = ranges::range_difference_t<decltype((r))>;
    static_assert(convertible_to<decltype((count)), D>::value, "Constraints not satisfied");
    return (*this)(std::forward<R>(r), count, category<R&&, T, D>{});
  }

  template<typename DifferenceType>
  constexpr auto operator()(DifferenceType&& count) const {
    return range_adaptor<take_niebloid, std::remove_reference_t<DifferenceType>>(std::forward<DifferenceType>(count));
  }
};
} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::take_niebloid take{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_TAKE_H_
